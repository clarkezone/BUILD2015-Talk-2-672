#include "pch.h"

GridRoom::GridRoom()
{
    m_cWidth = 0;
    m_cHeight = 0;

    m_cAdjoiningRooms = 0;
}

void 
GridRoom::Set(
    int iGridOffsetX,
    int iGridOffsetY,
    int cWidth,
    int cHeight,
    int* rgEntries
    )
{
    m_rgEntries = new int[cWidth*cHeight];

    memcpy(
        m_rgEntries,
        rgEntries,
        cWidth*cHeight*sizeof(int)
        );

    m_cWidth = cWidth;
    m_cHeight = cHeight;

    m_iGridOffsetX = iGridOffsetX;
    m_iGridOffsetY = iGridOffsetY;
}

int 
GridRoom::GetEntry(
    float3 worldPos
    )
{
    if (worldPos.x < 0.0f)
    {
        worldPos.x -= 10000.0f;
    }

    if (worldPos.y < 0.0f)
    {
        worldPos.y -= 10000.0f;
    }

    int iGridX = static_cast<int>(worldPos.x / 10000.0f);
    int iGridY = static_cast<int>(worldPos.y / 10000.0f);

    return GetEntry(iGridX - m_iGridOffsetX, iGridY - m_iGridOffsetY);
}

bool 
GridRoom::WithinBounds(
    float3 worldPos
    )
{
    if (worldPos.x < 0.0f)
    {
        worldPos.x -= 10000.0f;
    }

    if (worldPos.y < 0.0f)
    {
        worldPos.y -= 10000.0f;
    }

    int iGridX = static_cast<int>(worldPos.x / 10000.0f);
    int iGridY = static_cast<int>(worldPos.y / 10000.0f);

    iGridX -= m_iGridOffsetX;
    iGridY -= m_iGridOffsetY;

    return iGridX >= 0 &&
        iGridY >= 0 &&
        iGridX < m_cWidth &&
        iGridY < m_cHeight;
}

int
GridRoom::GetEntry(
    int iGridX,
    int iGridY
    )
{
    if (iGridX < 0 ||
        iGridY < 0 ||
        iGridX >= m_cWidth ||
        iGridY >= m_cHeight
        )
    {
        return 2;
    }
    else
    {
//        assert(m_rgEntries[iGridY * m_cWidth + iGridX] != 1);

        return m_rgEntries[iGridY * m_cWidth + iGridX];
    }

}


void 
GridRoom::AppendVisuals(
    Compositor^ spCompositor,
    ContainerVisual^ spRoomRoot,
    float3 cameraLookVector,
	ICompositionSurface^ materials[]
    )
{
    /*
    SolidColorVisual^ xRect = spCompositor->CreateSolidColorVisual();
    SolidColorVisual^ yRect = spCompositor->CreateSolidColorVisual();
    SolidColorVisual^ zRect = spCompositor->CreateSolidColorVisual();
    SolidColorVisual^ zIncreaseRect = spCompositor->CreateSolidColorVisual();

    zRect->Color = Windows::UI::Colors::Blue;
    zRect->Size = float2(10000.0f, 10000.0f);
    zRect->Offset = float3(0.0f, 0.0f, 0.0f);
    zRect->TransformMatrix = make_float4x4_translation(0.0f, 0.0f, 0.0f);

    zIncreaseRect->Color = Windows::UI::Colors::Purple;
    zIncreaseRect->Size = float2(10000.0f, 10000.0f);
    zIncreaseRect->Offset = float3(0.0f, 0.0f, 10000.0f);
    zIncreaseRect->TransformMatrix = make_float4x4_translation(0.0f, 0.0f, 0.0f);

    xRect->Color = Windows::UI::Colors::Red;
    xRect->Size = float2(10000.0f, 10000.0f);
    xRect->Offset = float3(0.0f, 0.0f, 0.0f);
    xRect->TransformMatrix = make_float4x4_rotation_x(DirectX::XM_PIDIV2);

    yRect->Color = Windows::UI::Colors::Yellow;
    yRect->Size = float2(10000.0f, 10000.0f);
    yRect->Offset = float3(0.0f, 0.0f, 10000.0f);
    yRect->TransformMatrix = make_float4x4_rotation_y(DirectX::XM_PIDIV2);

    spRoomRoot->Children->InsertAtTop(xRect);
    spRoomRoot->Children->InsertAtTop(yRect);
    spRoomRoot->Children->InsertAtTop(zRect);
    spRoomRoot->Children->InsertAtTop(zIncreaseRect);
    */

    int iStartX;
    int iEndX;
    int iStepX;

    int iStartY;
    int iEndY;
    int iStepY;

    if (cameraLookVector.x > 0.0f)
    {
        iStartX = m_cWidth - 1;
        iEndX = -1;
        iStepX = -1;
    }
    else
    {
        iStartX = 0;
        iEndX = m_cWidth;
        iStepX = 1;
    }

    if (cameraLookVector.y > 0.0f)
    {
        iStartY = m_cHeight - 1;
        iEndY = -1;
        iStepY = -1;
    }
    else
    {
        iStartY = 0;
        iEndY = m_cHeight;
        iStepY = 1;
    }

    if (fabsf(cameraLookVector.x) > fabsf(cameraLookVector.y))
    {
        for (int iGridX = iStartX; iGridX != iEndX; iGridX += iStepX)
        {
            for (int iGridY = iStartY; iGridY != iEndY; iGridY += iStepY)
            {
                AddGridElementVisuals(
                    spCompositor,
                    spRoomRoot,
                    materials,
                    cameraLookVector,
                    iGridX,
                    iGridY
                    );
            }
        }
    }
    else
    {
        for (int iGridY = iStartY; iGridY != iEndY; iGridY += iStepY)
        {
            for (int iGridX = iStartX; iGridX != iEndX; iGridX += iStepX)
            {
                AddGridElementVisuals(
                    spCompositor,
                    spRoomRoot,
                    materials,
                    cameraLookVector,
                    iGridX,
                    iGridY
                    );
            }
        }
    }
}

BasicRoom::BasicRoom()
{}

void 
BasicRoom::AddVisual(
    __in Compositor^ spCompositor,
	__in ICompositionSurface^ materials[],
    __in ContainerVisual^ spRoomRoot,
    __in float2 size,
    __in float3 offset,
    __in float4x4 transform,
    __in int materialId
    )
{
    bool fLighting = false;

    if (materialId != -1)
    {
        fLighting = true;
    }
    else
    {
        materialId = 3;
    }

    ImageVisual^ rect = spCompositor->CreateImageVisual();
	rect->Size = size;
	rect->Offset = offset;
	rect->TransformMatrix = transform;
	rect->Image = materials[materialId];
	rect->Stretch = CompositionStretch::Fill;

    if (fLighting == false)
    {
        OpenDoor(
            spCompositor,
            rect,
            5000
            );

        spRoomRoot->Children->InsertAtBottom(rect);
    }
    else
    {
        spRoomRoot->Children->InsertAtTop(rect);
    }

    if (fLighting)
    {
        if (materialId == 0 ||
            materialId == 1 ||
            materialId == 3
            )
        {
            SolidColorVisual^ lightingVisual = spCompositor->CreateSolidColorVisual();

            Windows::UI::Color lightColor;

            lightColor.A = 100;
            lightColor.R = 0;
            lightColor.G = 0;
            lightColor.B = 0;

            lightingVisual->Size = size;
            lightingVisual->Offset = offset;
            lightingVisual->Color = lightColor;
            lightingVisual->TransformMatrix = transform;

            spRoomRoot->Children->InsertAtTop(lightingVisual);
        }

        if (materialId == 2)
        {
            ImageVisual^ lightingVisual = spCompositor->CreateImageVisual();

            lightingVisual->Size = size;
            lightingVisual->Offset = offset;
            lightingVisual->TransformMatrix = transform;
            lightingVisual->Image = materials[4];
            lightingVisual->Stretch = CompositionStretch::Fill;
            spRoomRoot->Children->InsertAtTop(lightingVisual);

            Flicker(spCompositor, lightingVisual, 15000000L);
        }
    }
}

void
GridRoom::OpenDoor(
    __in Compositor^ spCompositor,
    __in ImageVisual^ spVisual,
    __in LONG duration
    )
{
    LinearEasingFunction^ linearEasing = spCompositor->CreateLinearEasingFunction();
    Vector3KeyFrameAnimation^ offsetAnimation = spCompositor->CreateVector3KeyFrameAnimation();

    Windows::Foundation::Numerics::float3 offset2 = spVisual->Offset;
    offset2.z -= 10000;

    offsetAnimation->InsertKeyFrame(0.000f, spVisual->Offset, linearEasing);
    offsetAnimation->InsertKeyFrame(0.250f, offset2, linearEasing);
    offsetAnimation->InsertKeyFrame(0.750f, offset2, linearEasing);
    offsetAnimation->InsertKeyFrame(1.000f, spVisual->Offset, linearEasing);

    Windows::Foundation::TimeSpan timeSpan;
    timeSpan.Duration = duration * 10000;

    offsetAnimation->Duration = timeSpan;

    m_pDoorAnimation = spVisual->ConnectAnimation("Offset", offsetAnimation);
}

void
GridRoom::Flicker(
	__in Compositor^ spCompositor,
	__in ImageVisual^ spVisual,
	__in LONG duration
	)
{
	CubicBezierEasingFunction^ cubicEasing = spCompositor->CreateCubicBezierEasingFunction(float2(.53f, .09f), float2(.43f, .91f));
	ScalarKeyFrameAnimation^ opacityAnimation = spCompositor->CreateScalarKeyFrameAnimation();

	opacityAnimation->InsertKeyFrame(0.000f, 0.5f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.100f, 0.6f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.200f, 0.7f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.300f, 0.8f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.400f, 0.9f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.500f, 1.0f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.600f, 0.9f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.700f, 0.8f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.800f, 0.7f, cubicEasing);
	opacityAnimation->InsertKeyFrame(0.900f, 0.6f, cubicEasing);
	opacityAnimation->InsertKeyFrame(1.0f, 0.5f, cubicEasing);

	Windows::Foundation::TimeSpan timeSpan;
	timeSpan.Duration = duration;

    Windows::Foundation::TimeSpan delayTimeSpan;
    delayTimeSpan.Duration = duration* static_cast<float>(rand()%1000)/1000.0f;

	opacityAnimation->Duration = timeSpan;
	opacityAnimation->RepeatCount = -1;
    opacityAnimation->DelayTime = delayTimeSpan;

	spVisual->ConnectAnimation("Opacity", opacityAnimation)->Start();
}

void 
GridRoom::AddAdjoiningRoom(
    GridRoom* pAdjoiningRoom,
    int iRoomId
    )
{
    assert(m_cAdjoiningRooms < MAX_ROOMS);

    m_rgAdjoiningRooms[m_cAdjoiningRooms] = pAdjoiningRoom;
    m_rgAdjoiningRoomIds[m_cAdjoiningRooms] = iRoomId;

    m_cAdjoiningRooms++;
}

void 
GridRoom::AddGridElementVisuals(
    Compositor^ spCompositor,
    ContainerVisual^ spRoomRoot,
    ICompositionSurface^ materials[],
    float3 cameraLookVector,
    int iGridX,
    int iGridY
    )
{
    float2 tileSize = float2(10000.0f, 10000.0f);

    if (GetEntry(iGridX, iGridY) <= 0)
    {
        AddVisual(
            spCompositor,
            materials,
            spRoomRoot,
            tileSize,
            float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
            make_float4x4_translation(0.0f, 0.0f, 0.0f),
            0   // 0 hardcodes to floor
            );

        AddVisual(
            spCompositor,
            materials,
            spRoomRoot,
            tileSize,
            float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, 5000.0f),
            make_float4x4_translation(0.0f, 0.0f, 0.0f),
            1   // 1 hardcodes to ceiling
            );

        if (GetEntry(iGridX, iGridY) < 0)
        {
            if (GetEntry(iGridX, iGridY - 1) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    -1
                    );
            }

            if (GetEntry(iGridX, iGridY + 1) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY + 1)*10000.0f, -5000.0f),
                    make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    -1
                    );
            }

            if (GetEntry(iGridX - 1, iGridY) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_y(DirectX::XM_PIDIV2) * make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    -1
                    );
            }

            if (GetEntry(iGridX + 1, iGridY) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX + 1)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_y(DirectX::XM_PIDIV2) * make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    -1
                    );
            }
        }
        else
//        if (GetEntry(iGridX, iGridY) >= 0)
        {
            if (GetEntry(iGridX, iGridY - 1) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    GetEntry(iGridX, iGridY - 1)
                    );
            }

            if (GetEntry(iGridX, iGridY + 1) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY + 1)*10000.0f, -5000.0f),
                    make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    GetEntry(iGridX, iGridY + 1)
                    );
            }

            if (GetEntry(iGridX - 1, iGridY) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_y(DirectX::XM_PIDIV2) * make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    GetEntry(iGridX - 1, iGridY)
                    );
            }

            if (GetEntry(iGridX + 1, iGridY) > 0)
            {
                AddVisual(
                    spCompositor,
                    materials,
                    spRoomRoot,
                    tileSize,
                    float3((iGridX + m_iGridOffsetX + 1)*10000.0f, (iGridY + m_iGridOffsetY)*10000.0f, -5000.0f),
                    make_float4x4_rotation_y(DirectX::XM_PIDIV2) * make_float4x4_rotation_x(DirectX::XM_PIDIV2),
                    GetEntry(iGridX + 1, iGridY)
                    );
            }
        }
    }
}
