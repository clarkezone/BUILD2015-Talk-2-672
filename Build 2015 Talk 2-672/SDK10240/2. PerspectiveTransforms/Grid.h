#pragma once

#define MAX_ROOMS 24

namespace Orientation
{
    enum Enum
    {
        LookDownPositiveX,
        LookDownPositiveY,
        LookDownPositiveZ,
        LookDownNegativeX,
        LookDownNegativeY,
        LookDownNegativeZ,
        Total
    };
};

class GridRoom
{
public:
    GridRoom();

    void ResetLighting();

    void Set(
        int iGridOffsetX,
        int iGridOffsetY,
        int cWidth,
        int cHeight,
        int* entries
        );

    void AppendVisuals(
        Compositor^ spCompositor,
        ContainerVisual^ spRoomRoot,
        float3 cameraLookVector,
		ICompositionSurface^ materials[]
        );

    int GetEntry(
        int iGridX,
        int iGridY
        );

    int GetEntry(
        float3 worldPos
        );

    bool WithinBounds(
        float3 worldPos
        );

    virtual void AddVisual(
        __in Compositor^ spCompositor,
		__in ICompositionSurface^ materials[],
        __in ContainerVisual^ spRoomRoot,
        __in float2 size,
        __in float3 offset,
        __in float4x4 transforms,
        __in int materialId
        ) PURE;

    void Flicker(
        __in Compositor^ spCompositor,
        __in ImageVisual^ spVisual,
        __in LONG duration
        );

    void AddAdjoiningRoom(
        GridRoom* pAdjoiningRoom,
        int iRoomId
        );

    int GetNumAdjoiningRooms() const
    {
        return m_cAdjoiningRooms;
    }

    int GetAdjoiningRoomId(
        int iEntry
        ) const
    {
        return m_rgAdjoiningRoomIds[iEntry];
    }

    void PlayDoor()
    {
        if (m_pDoorAnimation)
        {
            m_pDoorAnimation->Start();
        }
    }

protected:
    void AddGridElementVisuals(
        Compositor^ spCompositor,
        ContainerVisual^ spRoomRoot,
        ICompositionSurface^ materials[],
        float3 cameraLookVector,
        int iGridX,
        int iGridY
        );

    void OpenDoor(
        __in Compositor^ spCompositor,
        __in ImageVisual^ spVisual,
        __in LONG duration
        );

private:
    int* m_rgEntries;
    int m_cWidth;
    int m_cHeight;

    int m_iGridOffsetX;
    int m_iGridOffsetY;

    GridRoom* m_rgAdjoiningRooms[MAX_ROOMS];
    int m_rgAdjoiningRoomIds[MAX_ROOMS];
    int m_cAdjoiningRooms;

protected:
    SolidColorVisual^ _lightingVisuals[1000];
    int _cLightingVisuals;

    CompositionPropertyAnimator^ m_pDoorAnimation;
};

class BasicRoom : public GridRoom
{
public:
    BasicRoom();

    virtual void AddVisual(
        __in Compositor^ spCompositor,
		__in ICompositionSurface^ materials[],
        __in ContainerVisual^ spRoomRoot,
        __in float2 size,
        __in float3 offset,
        __in float4x4 transforms,
        __in int materialId
        ) override;
};