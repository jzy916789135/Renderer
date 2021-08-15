
#include "test_camera.h"
#include "render.h"
#include <queue>
#include <unordered_set>

int main()
{
    Render render;

    render.Init();

    //int nx = renderer.smokes["smoke"]->nx, ny = renderer.smokes["smoke"]->ny, nz = renderer.smokes["smoke"]->nz;
    //ThreadGuard phyx(std::thread(SmokeSolver::Run, std::ref(renderer.smokes["smoke"]->density), std::ref(renderer.phyxMutex), std::ref(renderer.exit), nx, ny, nz,
    //	std::ref(renderer.smokes["smoke"]->force.x), std::ref(renderer.smokes["smoke"]->force.y), std::ref(renderer.smokes["smoke"]->force.z),
    //	std::ref(renderer.smokes["smoke"]->decay)), ThreadGuard::DesAction::JOIN);

    render.Draw();

    return 0;
}