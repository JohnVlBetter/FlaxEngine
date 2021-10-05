// Copyright (c) 2012-2021 Wojciech Figat. All rights reserved.

#pragma once

#if USE_EDITOR

#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Shader.h"
#include "Engine/Content/Assets/Texture.h"
#include "Engine/Graphics/Materials/IMaterial.h"

class GPUPipelineState;

/// <summary>
/// Rendering Level Of Detail number as colors to debug LOD switches in editor.
/// </summary>
class LODPreviewMaterialShader : public IMaterial
{
private:

    AssetReference<Shader> _shader;
    GPUPipelineState* _ps = nullptr;
    MaterialInfo _info;

public:

    LODPreviewMaterialShader();
    virtual ~LODPreviewMaterialShader()
    {
    }

private:

#if COMPILE_WITH_DEV_ENV
    void OnShaderReloading(Asset* obj);
#endif

public:

    // [IMaterial]
    const MaterialInfo& GetInfo() const override;
    bool IsReady() const override;
    DrawPass GetDrawModes() const override;
    void Bind(BindParameters& params) override;
};

#endif