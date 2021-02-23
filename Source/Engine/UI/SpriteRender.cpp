// Copyright (c) 2012-2021 Wojciech Figat. All rights reserved.

#include "SpriteRender.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Core/Math/OrientedBoundingBox.h"
#include "Engine/Level/Scene/SceneRendering.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/Assets/Model.h"
#include "Engine/Content/Assets/MaterialInstance.h"
#include "Engine/Level/Actors/Camera.h"
#include "Engine/Serialization/Serialization.h"

SpriteRender::SpriteRender(const SpawnParams& params)
    : Actor(params)
    , _color(Color::White)
    , _size(100.0f)
{
    _quadModel = Content::LoadAsyncInternal<Model>(TEXT("Engine/Models/Quad"));
    Material.Loaded.Bind<SpriteRender, &SpriteRender::OnMaterialLoaded>(this);
    Image.Changed.Bind<SpriteRender, &SpriteRender::OnImageChanged>(this);
}

Vector2 SpriteRender::GetSize() const
{
    return _size;
}

void SpriteRender::SetSize(const Vector2& value)
{
    if (_size == value)
        return;
    _size = value;
    OnTransformChanged();
}

Color SpriteRender::GetColor() const
{
    return _color;
}

void SpriteRender::SetColor(const Color& value)
{
    _color = value;
    if (_paramColor)
        _paramColor->SetValue(value);
}

void SpriteRender::OnMaterialLoaded()
{
    // Setup material instance
    if (!_materialInstance)
    {
        _materialInstance = Content::CreateVirtualAsset<MaterialInstance>();
        _materialInstance->AddReference();
    }
    _materialInstance->SetBaseMaterial(Material);

    // Cache parameters
    _paramImage = _materialInstance->GetParameter(TEXT("Image"));
    if (_paramImage && _paramImage->GetParameterType() != MaterialParameterType::Texture)
        _paramImage = nullptr;
    else if (_paramImage)
        _paramImage->SetValue(Image.Get());
    _paramColor = _materialInstance->GetParameter(TEXT("Color"));
    if (_paramColor && _paramColor->GetParameterType() != MaterialParameterType::Color && _paramColor->GetParameterType() != MaterialParameterType::Vector4 && _paramColor->GetParameterType() != MaterialParameterType::Vector3)
        _paramColor = nullptr;
    else if (_paramColor)
        _paramColor->SetValue(_color);
}

void SpriteRender::OnImageChanged()
{
    if (_paramImage)
        _paramImage->SetValue(Image.Get());
}

bool SpriteRender::HasContentLoaded() const
{
    return (Material == nullptr || Material->IsLoaded()) && (Image == nullptr || Image->IsLoaded());
}

void SpriteRender::Draw(RenderContext& renderContext)
{
    if (!Material || !Material->IsLoaded() || !_quadModel || !_quadModel->IsLoaded())
        return;
    auto model = _quadModel.As<Model>();
    if (model->GetLoadedLODs() == 0)
        return;
    auto& view = renderContext.View;
    Matrix m1, m2, m3, world;
    Matrix::Scaling(_size.X, _size.Y, 1.0f, m2);
    Matrix::RotationY(PI, m3);
    Matrix::Multiply(m2, m3, m1);
    if (FaceCamera)
    {
        Matrix::Billboard(_transform.Translation, view.Position, Vector3::Up, view.Direction, m2);
        Matrix::Multiply(m1, m2, m3);
        Matrix::Scaling(_transform.Scale, m1);
        Matrix::Multiply(m1, m3, world);
    }
    else
    {
        _transform.GetWorld(m2);
        Matrix::Multiply(m1, m2, world);
    }
    model->LODs[0].Draw(renderContext, _materialInstance, world, GetStaticFlags(), false, DrawModes, GetPerInstanceRandom());
}

void SpriteRender::DrawGeneric(RenderContext& renderContext)
{
    Draw(renderContext);
}

void SpriteRender::Serialize(SerializeStream& stream, const void* otherObj)
{
    // Base
    Actor::Serialize(stream, otherObj);

    SERIALIZE_GET_OTHER_OBJ(SpriteRender);

    SERIALIZE_MEMBER(Size, _size);
    SERIALIZE_MEMBER(Color, _color);
    SERIALIZE(Image);
    SERIALIZE(Material);
    SERIALIZE(FaceCamera);
    SERIALIZE(DrawModes);
}

void SpriteRender::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
    // Base
    Actor::Deserialize(stream, modifier);

    DESERIALIZE_MEMBER(Size, _size);
    DESERIALIZE_MEMBER(Color, _color);
    DESERIALIZE(Image);
    DESERIALIZE(Material);
    DESERIALIZE(FaceCamera);
    DESERIALIZE(DrawModes);

    if (_paramImage)
        _paramImage->SetValue(Image.Get());
    if (_paramColor)
        _paramColor->SetValue(_color);
}

void SpriteRender::OnEndPlay()
{
    // Base
    Actor::OnEndPlay();

    // Release material instance
    if (_materialInstance)
    {
        _materialInstance->SetBaseMaterial(nullptr);
        _materialInstance->Params.Resize(0);
        _materialInstance->RemoveReference();
        _materialInstance = nullptr;
    }
}

void SpriteRender::OnEnable()
{
    GetSceneRendering()->AddGeometry(this);

    // Base
    Actor::OnEnable();
}

void SpriteRender::OnDisable()
{
    GetSceneRendering()->RemoveGeometry(this);

    // Base
    Actor::OnDisable();
}

void SpriteRender::OnTransformChanged()
{
    // Base
    Actor::OnTransformChanged();

    const BoundingSphere localSphere(Vector3::Zero, _size.Length());
    Matrix world;
    _transform.GetWorld(world);
    BoundingSphere::Transform(localSphere, world, _sphere);
    BoundingBox::FromSphere(_sphere, _box);
}
