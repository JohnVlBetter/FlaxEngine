﻿// Copyright (c) 2012-2022 Wojciech Figat. All rights reserved.

#pragma once

#include "Types.h"
#include "Engine/Core/Types/Pair.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/ScriptingType.h"

/// <summary>
/// High-level networking replication system for game objects.
/// </summary>
API_CLASS(static, Namespace = "FlaxEngine.Networking") class FLAXENGINE_API NetworkReplicator
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(NetworkReplicator);

public:
    typedef void (*SerializeFunc)(void* instance, NetworkStream* stream);
    typedef Pair<SerializeFunc, SerializeFunc> SerializeFuncPair;
    static SerializeFuncPair GetSerializer(const ScriptingTypeHandle& typeHandle);

    /// <summary>
    /// Global table for registered types serialization methods (key is type name, value is pair of methods to serialize and deserialize object).
    /// </summary>
    static Dictionary<ScriptingTypeHandle, SerializeFuncPair> SerializersTable;

public:
    /// <summary>
    /// Adds the object to the network replication system.
    /// </summary>
    /// <remarks>Does nothing if network is offline.</remarks>
    /// <param name="obj">The object to replicate.</param>
    /// <param name="owner">The owner of the object (eg. player that spawned it).</param>
    API_FUNCTION() static void AddObject(ScriptingObject* obj, ScriptingObject* owner);
};