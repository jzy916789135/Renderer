#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec3 fragWorldPos;
    vec3 fragWorldNormal;
    vec2 fragTexC;
    vec3 T;
    vec3 B;
    vec3 N;
    vec4 shadowCoord;
} gs_in[];

out vec2 TEXC;
uniform float time;
vec3 GetNormal()
{
    vec3 a  = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b  = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}
vec4 explode(vec4 position,vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}
void main()
{
    vec3 normal = GetNormal();
    gl_Position = explode(gl_in[0].gl_Position, normal);
    TEXC = gs_in[0].fragTexC;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TEXC = gs_in[1].fragTexC;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TEXC = gs_in[2].fragTexC;
    EmitVertex();

    EndPrimitive();
}
