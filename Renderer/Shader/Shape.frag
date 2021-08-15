#version 330 core
out vec4 FragColor;

//in vec2 TEXC;
in VS_OUT
{
    vec3 fragWorldPos;
	vec3 fragWorldNormal;
	vec2 fragTexC;
	vec3 T;
    vec3 B;
    vec3 N;
	vec4 shadowCoord;
} fs_in;


struct Light
{
	int type;
	vec3 pos;
	vec3 strength;
	vec3 dir;
	float fallStart;
	float fallEnd;
	float spotPower;
};

struct PassCb
{
	mat4 view;
	mat4 proj;
	vec3 eyePos;
	vec4 ambientLight;
	int lightNum;
};

struct Material
{
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
	bool useDiffuseMap;
	bool useNormalMap;
};

uniform Light light[2];
uniform PassCb passCb;
uniform Material material;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform int textureScale;
#define PI 3.141592653589793
#define PBR false

//基于物理的光照模型

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

//V = Wo   L = wi(入射方向)
vec3 Cook_Torrance(Light light, vec3 L, vec3 N, vec3 V)
{
	//平面对于法向入射的反应 0.04指的是塑料
	vec3 F0 = vec3(0.04);
	F0 = mix(F0,material.albedo,material.metallic);

	//反射方程
	vec3 Lo = vec3(0.0f);
	//计算radiance
	vec3 H = normalize(V+L);
	float distance = length(light.pos - fs_in.fragWorldPos);
	float attenuation = 1.0 / (distance * distance);
	//vec3 radiance = light.strength * attenuation;
	vec3 radiance = light.strength;
	//BEDF
	
	float NDF = DistributionGGX(N, H, material.roughness);
	float G   = GeometrySmith(V,N, L,material.roughness);
	vec3 F   = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
	/*
	float NDF = 0.5f;
	float G   = 0.5f;
	vec3 F   = vec3(0.5f);
	*/
	vec3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = nominator / max(denominator, 0.001); 

	//KS表示光能中被反射的能量比例
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	//如果是金属就不会有漫反射
	kD *= 1.0 - material.metallic;

	//缩放辐射强度
	float NdotL = max(dot(N,L), 0.0);
	
	if (material.useDiffuseMap) Lo = (kD * material.albedo * texture(diffuseMap, fs_in.fragTexC * textureScale).xyz / PI + specular) * radiance * NdotL;
	else Lo = (kD * material.albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = vec3(0.03) * material.albedo * material.ao;

	vec3 color = ambient + Lo;
	
	return color;
}


//基于经验的光照模型BlinnPhong
vec3 BlinnPhong(vec3 lightStrength, vec3 lightDir, vec3 normal, vec3 eyeDir)
{
	float shininess = (1.0 - material.roughness) * 255;
	//diffuse shading漫反射系数
	float diff = max(dot(lightDir, normal),0.0);
	//specular 镜面反射系数
	vec3 halfV = (normalize(lightDir) + normalize(eyeDir) ) / 2.0;
	float spec = pow(max(dot(halfV, normal), 0.0), shininess);
	//combine
	vec3 diffuse = vec3(0.0f);
	if(material.useDiffuseMap) diffuse = diff * lightStrength * material.albedo.xyz * texture(diffuseMap, fs_in.fragTexC * textureScale).xyz;
	else diffuse = diff * lightStrength * material.albedo.xyz;

	vec3 specular = spec * lightStrength * material.albedo.xyz;

	return passCb.ambientLight.xyz + diffuse + specular;

}

vec3 CalDirLight(Light light, vec3 normal)
{
	vec3 L = normalize(light.pos - fs_in.fragWorldPos);
	vec3 V = normalize(passCb.eyePos - fs_in.fragWorldPos);
	vec3 N = normal;
	if(PBR)
		return Cook_Torrance(light, L, N, V);
	
	return BlinnPhong(light.strength, L, normal, V);
}

vec3 CalPointLight(Light light, vec3 normal)
{
	vec3 lightDir = normalize(light.pos - fs_in.fragWorldPos);
	vec3 eyeDir = normalize(passCb.eyePos - fs_in.fragWorldPos);
	float lightLen = length(light.pos - fs_in.fragWorldPos);
	//光照衰减
	vec3 strength = light.strength / pow(lightLen, 2);
	return BlinnPhong(strength, lightDir, normal, eyeDir);

}



//--shadow---------------------------
#define NUM_SAMPLES 40
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define SHADOW_HARDNESS 800
#define EPS 1e-3

vec2 poissonDisk[NUM_SAMPLES];

float unpack(vec4 rgbaDepth)
{
	const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
	return dot(rgbaDepth, bitShift);
}


highp float rand_2to1(vec2 uv ) { 
    // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );//mod dt除以PI所得的余数
	return fract(sin(sn) * c);//fract得到这个值得小数部分
}


void poissonDiskSamples( const in vec2 randomSeed ) {

  float ANGLE_STEP = 2 * PI * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI * 2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}


float showShadowMap(sampler2D shadowMap, vec4 shadowCoord)
{
	// 执行透视除法并标准化
	vec3 coord = shadowCoord.xyz / shadowCoord.w * 0.5 + 0.5;
	float closestDepth = texture(shadowMap,coord.xy).r;
	//float closestDepth = unpack(texture(shadowMap,coord.xy));
	return closestDepth;
}

float PCF(sampler2D shadowMap, vec4 shadowCoord)
{
	vec3 coord = shadowCoord.xyz / shadowCoord.w * 0.5 + 0.5;
	poissonDiskSamples(coord.xy);
	float visibility = 0.0;
	for(int i = 0; i < PCF_NUM_SAMPLES; i++)
	{
		//float closestDepth = texture(shadowMap,coord.xy + poissonDisk[i] /  SHADOW_HARDNESS).r;
		float closestDepth = unpack(texture2D(shadowMap, coord.xy + poissonDisk[i] / SHADOW_HARDNESS));
		if (closestDepth + EPS > coord.z || closestDepth == 0.0) visibility += 1.0;
	}
	return visibility /= float(PCF_NUM_SAMPLES);

}
//oprngl 教程的PCF方法
float PCF1(sampler2D shadowMap, vec4 shadowCoord)
{
	vec3 coord = shadowCoord.xyz / shadowCoord.w * 0.5 + 0.5;
	float closestDepth = texture(shadowMap,coord.xy).r;
	float currentDepth = coord.z;

	vec3 normal = normalize(fs_in.fragWorldNormal);
	vec3 lightDir = normalize(light[0].pos - fs_in.fragWorldPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float shadow = 0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, coord.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
	shadow /= 9.0;

	if(coord.z > 1.0)
		shadow = 0.0;
	return shadow;
}


void main()
{	
	vec3 color = vec3(0.0f);
	vec3 normal = vec3(0.0f);
	for (int i = 0; i < passCb.lightNum; ++i)
	{	
		if(material.useNormalMap)
		{
			normal = texture(normalMap, fs_in.fragTexC * textureScale).rgb;
			normal = normalize(normal * 2.0 - 1.0);
			mat3 TBN = mat3(normalize(fs_in.T),normalize(fs_in.B),normalize(fs_in.N));
			normal = normalize(TBN * normal);
		}
		else
			normal = normalize(fs_in.fragWorldNormal);
		if(light[i].type == 0)	color += CalDirLight(light[i],normal);
		else if (light[i].type == 1) color += CalPointLight(light[i], normal);

	}
	
	float visibility;
	visibility = PCF(shadowMap, fs_in.shadowCoord);
	//阴影系数
	float shadow = PCF1(shadowMap, fs_in.shadowCoord);


	color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0/2.2));

	//FragColor = vec4(vec3(color * visibility), 1.f);

	FragColor = vec4(vec3(color * (1-shadow)), 1.f);
	

}