#include "Level.h"

#include <sstream>
#include <Utility/Resource.h>
#include <Math/Vector2.h>
#include <Swizzle.h>
#include <SceneGraph/Scene.h>

#include "FloorTile.h"
#include "WallBrick.h"

using namespace std;

namespace PushTheBox { namespace Game {

Level* Level::_current = nullptr;

Level* Level::current() {
    CORRADE_ASSERT(_current, "No Level instance created", nullptr);
    return _current;
}

Level::Level(const std::string& name, Scene3D* scene, SceneGraph::DrawableGroup<3>* drawables) {
    CORRADE_ASSERT(!_current, "Only one Level instance can be created at a time", );
    _current = this;

    /* Get level data */
    Corrade::Utility::Resource rs("PushTheBoxData");
    std::istringstream in(rs.get("levels/" + name + ".txt"));

    /* Level size on first line */
    in >> _size.x() >> _size.y();
    level.resize(_size.product(), TileType::Empty);
    CORRADE_ASSERT(_size > Math::Vector2<int>(3, 3), "Level" << name << "is too small:" << _size, );
    CORRADE_INTERNAL_ASSERT(in.peek() == '\n');
    in.ignore();

    /* Sanity checks */
    size_t boxCount = 0;
    size_t targetCount = 0;

    /* Parse the file */
    _startingPosition = {-1, -1};
    Math::Vector2<int> position;
    while(in.peek() > 0) {
        TileType type = {};
        switch(in.peek()) {
            /* Empty, already marked */
            case ' ': break;

            /* Wall */
            case '#': type = TileType::Wall; break;

            /* Starting position */
            case '@':
                CORRADE_ASSERT(_startingPosition == Math::Vector2<int>(-1, -1), "Multiple starting positions in level" << name, );
                _startingPosition = position;
                /* No break, as we need to mark it as floor */

            /* Floor */
            case '_': type = TileType::Floor; break;

            /* Box */
            case '$':
                type = TileType::Box;
                ++boxCount;
                break;

            /* Starting position on target */
            case '+':
                CORRADE_ASSERT(_startingPosition == Math::Vector2<int>(-1, -1), "Multiple starting positions in level" << name, );
                _startingPosition = position;
                /* No break, as we need to mark it as target */

            /* Target */
            case '.':
                type = TileType::Target;
                ++targetCount;
                break;

            /* Box on target */
            case '*':
                type = TileType::BoxOnTarget;
                ++boxCount;
                ++targetCount;
                break;

            /* New line */
            case '\n':
                in.ignore();
                position.x() = 0;
                ++position.y();
                continue;

            default:
                CORRADE_ASSERT(false, "Unknown character" << in.peek() << "in file of level" << name << "at position" << position, );
        }

        in.ignore();
        set(position, type, scene, drawables);
        ++position.x();
    }

    /* Sanity checks */
    CORRADE_ASSERT(_startingPosition != Math::Vector2<int>(-1, -1), "Level" << name << "has no starting position", );
    CORRADE_ASSERT(boxCount == targetCount, "Level" << name << "has" << boxCount << "boxes, but" << targetCount << "targets", );
}

Level::~Level() {
    CORRADE_INTERNAL_ASSERT(_current == this);
    _current = nullptr;
}

void Level::moveBox(const Math::Vector2<int>& from, const Math::Vector2<int>& to) {
    CORRADE_INTERNAL_ASSERT(at(from) == TileType::Box);

    at(from) = TileType::Floor;
    at(to) = TileType::Box;

    for(size_t i = 0; i < boxes.size(); ++i) {
        if(boxes[i]->position() != from) continue;

        boxes[i]->move(to-from);
        return;
    }

    CORRADE_INTERNAL_ASSERT(false);
}

void Level::set(const Math::Vector2<int>& position, TileType type, Scene3D* scene, SceneGraph::DrawableGroup<3>* drawables) {
    at(position) = type;

    Vector3 scenePosition = Vector3::from(swizzle<'x', '0', 'y'>(position));

    switch(type) {
        case TileType::Empty:
            break;
        case TileType::Box:
            boxes.push_back(new Box(position, scene, drawables));
            /* No break, as we need floor tile under it */
        case TileType::Floor:
            (new FloorTile(Color3<GLfloat>::fromHSV(0.0f,0.1f,0.3f), scene, drawables))
                ->translate(scenePosition);
            break;
        case TileType::BoxOnTarget:
            boxes.push_back(new Box(position, scene, drawables));
            /* No break, as we need target tile under it */
        case TileType::Target:
            (new FloorTile(Color3<GLfloat>::fromHSV(0.0f, 1.0f, 0.5f), scene, drawables))
                ->translate(scenePosition);
            break;
        case TileType::Wall:
            (new WallBrick(Color3<GLfloat>::fromHSV(0.0f, 0.2f, 1.0f), scene, drawables))
                ->translate(scenePosition);
            break;
    }
}

}}
