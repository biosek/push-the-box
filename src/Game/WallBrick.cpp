#include "WallBrick.h"

#include <Mesh.h>
#include <Swizzle.h>
#include <Shaders/PhongShader.h>

namespace PushTheBox { namespace Game {

WallBrick::WallBrick(const Vector2i& position, Object3D* parent, SceneGraph::DrawableGroup<3>* group): Object3D(parent), SceneGraph::Drawable<3>(this, group) {
    shader = SceneResourceManager::instance()->get<AbstractShaderProgram, Shaders::PhongShader>("phong");
    mesh = SceneResourceManager::instance()->get<Mesh>("wall-mesh");

    translate(Vector3(swizzle<'x', '0', 'y'>(position)));
}

void WallBrick::draw(const Matrix4& transformationMatrix, SceneGraph::AbstractCamera<3>*) {
    shader->setTransformationMatrix(transformationMatrix)
          ->setDiffuseColor(Color3<GLfloat>::fromHSV(Deg(30.0f), 0.2f, 1.0f))
          ->use();

    mesh->draw();
}

}}
