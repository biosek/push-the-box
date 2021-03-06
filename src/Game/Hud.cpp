#include "Game/Hud.h"

#include <SceneGraph/AbstractCamera.h>
#include <Text/Font.h>
#include <Text/TextRenderer.h>

namespace PushTheBox { namespace Game {

AbstractHudText::AbstractHudText(Object2D* parent, SceneGraph::DrawableGroup2D<>* drawables): Object2D(parent), Drawable<2>(this, drawables), shader(SceneResourceManager::instance()->get<AbstractShaderProgram, Shaders::TextShader2D>("text2d")), font(SceneResourceManager::instance()->get<Text::Font>("font")) {
    text = new Text::TextRenderer2D(*font, 0.06f);
}

void AbstractHudText::draw(const Matrix3& transformationMatrix, SceneGraph::AbstractCamera<2>* camera) {
    shader->setTransformationProjectionMatrix(camera->projectionMatrix()*transformationMatrix)
        ->setColor(Color3<>(1.0f))
        ->use();

    font->texture().bind(Shaders::TextShader2D::FontTextureLayer);

    text->mesh()->draw();
}

RemainingTargets::RemainingTargets(Object2D* parent, SceneGraph::DrawableGroup2D<>* drawables, SceneGraph::AnimableGroup2D<>* animables): AbstractHudText(parent, drawables), SceneGraph::Animable2D<>(this, animables) {
    text->reserve(32, Buffer::Usage::DynamicDraw, Buffer::Usage::StaticDraw);

    setDuration(0.4f);
    setRepeated(true);
    setRepeatCount(2);

    translate({-1.303f, -0.97f});
}

void RemainingTargets::update(uint32_t count) {
    text->render(std::to_string(count) + " remaining targets");

    setState(SceneGraph::AnimationState::Running);
}

void RemainingTargets::draw(const Matrix3& transformationMatrix, SceneGraph::AbstractCamera<2>* camera) {
    AbstractHudText::draw(transformationMatrix*Matrix3::scaling(Vector2(scale)), camera);
}

void RemainingTargets::animationStep(GLfloat time, GLfloat) {
    scale = 1.0f + Math::sin(Rad(Constants::pi()*time/duration()))*0.35f;
}

void RemainingTargets::animationStopped() {
    scale = 1.0f;
}

Moves::Moves(Object2D* parent, SceneGraph::DrawableGroup2D<>* drawables): AbstractHudText(parent, drawables) {
    text->reserve(32, Buffer::Usage::DynamicDraw, Buffer::Usage::StaticDraw);
}

void Moves::update(uint32_t count) {
    text->render(std::to_string(count) + " moves");

    resetTransformation();
    translate({1.303f - text->rectangle().width(), 0.97f - text->rectangle().top()});
}

}}
