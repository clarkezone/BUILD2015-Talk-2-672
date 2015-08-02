//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#include "pch.h"
#include "Main.h"

//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::Initialize
//
// This method is called during startup to associate the IFrameworkView with the
// CoreApplicationView.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::Initialize(CoreApplicationView ^ view)
{
	_view = view;
    _windowClosed = false;
    _windowVisible = true;
	_pointerDevice = nullptr;

    _currentRoomId = -1;

    for (int i = 0; i < ARRAYSIZE(_totalRooms); i++)
    {
        _totalRooms[i] = nullptr;
    }

    _cTotalRooms = 0;

    for (int i = 0; i < ARRAYSIZE(_currentRooms); i++)
    {
        _currentRooms[i] = nullptr;
    }

    for (int i = 0; i < ARRAYSIZE(_currentRoomRoots); i++)
    {
        _currentRoomRoots[i] = nullptr;
    }

    _cCurrentRooms = 0;

	_gestureRecognizer = ref new Windows::UI::Input::GestureRecognizer();

	_gestureRecognizer->GestureSettings =
		Windows::UI::Input::GestureSettings::ManipulationTranslateX |
		Windows::UI::Input::GestureSettings::ManipulationTranslateY |
		Windows::UI::Input::GestureSettings::ManipulationScale;

	_tokenManipulationStarted = _gestureRecognizer->ManipulationStarted::add(
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationStartedEventArgs^>(
			this, &OutlookBackgroundCx::OnManipulationStarted));

	_tokenManipulationUpdated = _gestureRecognizer->ManipulationUpdated::add(
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationUpdatedEventArgs^>(
			this, &OutlookBackgroundCx::OnManipulationUpdated));
	_tokenManipulationCompleted = _gestureRecognizer->ManipulationCompleted::add(
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Input::GestureRecognizer^, Windows::UI::Input::ManipulationCompletedEventArgs^>(
			this, &OutlookBackgroundCx::OnManipulationCompleted));
}

void OutlookBackgroundCx::SetDevice(_In_opt_ Windows::Devices::Input::IPointerDevice^ pointerDevice)
{
	_pointerDevice = pointerDevice;
}
//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::SetWindow
//
// This method is called when the CoreApplication has created a new CoreWindow,
// allowing the application to configure the window and start producing content
// to display.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::SetWindow(CoreWindow ^ window)
{
	_window = window;
	InitNewComposition();
	InitGraphics();
	window->PointerPressed +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(
			this, &OutlookBackgroundCx::OnPointerPressed);
	window->PointerReleased +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(
			this, &OutlookBackgroundCx::OnPointerReleased);
	window->PointerMoved +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(
			this, &OutlookBackgroundCx::OnPointerMoved);
}

//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::Load
//
// This method is called when a specific page is being loaded in the
// application.  It is not used for this application.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::Load(String ^)
{

}


//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::Run
//
// This method is called by CoreApplication::Run() to actually run the
// dispatcher's message pump.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::Run()
{
	_window->Activate();

    _window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &OutlookBackgroundCx::OnKeyDown);
    _window->KeyUp += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &OutlookBackgroundCx::OnKeyUp);

    while (!_windowClosed)
    {
        if (_windowVisible)
        {
            Render();
            _window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        }
        else
        {
            _window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

void 
OutlookBackgroundCx::OnKeyDown(
    __in CoreWindow^ pSender,
    __in KeyEventArgs^ e
    )
{
    switch (e->VirtualKey)
    {
    case Windows::System::VirtualKey::W:
        _moveForwardTriggered = true;
        break;

    case Windows::System::VirtualKey::S:
        _moveBackTriggered = true;
        break;

    case Windows::System::VirtualKey::A:
        _moveLeftTriggered = true;
        break;

    case Windows::System::VirtualKey::D:
        _moveRightTriggered = true;
        break;

    case Windows::System::VirtualKey::Right:
        _turnRightTriggered = true;
        break;

    case Windows::System::VirtualKey::Left:
        _turnLeftTriggered = true;
        break;
    }
}

void 
OutlookBackgroundCx::OnKeyUp(
    __in CoreWindow^ pSender,
    __in KeyEventArgs^ e
    )
{
    switch (e->VirtualKey)
    {
    case Windows::System::VirtualKey::W:
        _moveForwardTriggered = false;
        break;

    case Windows::System::VirtualKey::S:
        _moveBackTriggered = false;
        break;

    case Windows::System::VirtualKey::A:
        _moveLeftTriggered = false;
        break;

    case Windows::System::VirtualKey::D:
        _moveRightTriggered = false;
        break;

    case Windows::System::VirtualKey::Right:
        _turnRightTriggered = false;
        break;

    case Windows::System::VirtualKey::Left:
        _turnLeftTriggered = false;
        break;
    }
}

void OutlookBackgroundCx::OnPointerPressed(
	_In_ Windows::UI::Core::CoreWindow^,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{
	_gestureRecognizer->ProcessDownEvent(args->CurrentPoint);

}

void OutlookBackgroundCx::OnPointerReleased(
	_In_ Windows::UI::Core::CoreWindow^,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{
	_gestureRecognizer->ProcessUpEvent(args->CurrentPoint);

}

void OutlookBackgroundCx::OnPointerMoved(
	_In_ Windows::UI::Core::CoreWindow^,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{

	Windows::Foundation::Collections::IVector<Windows::UI::Input::PointerPoint^>^ pointerPoints = Windows::UI::Input::PointerPoint::GetIntermediatePoints(
		args->CurrentPoint->PointerId);
	_gestureRecognizer->ProcessMoveEvents(pointerPoints);
}


void OutlookBackgroundCx::OnManipulationStarted(
	_In_ Windows::UI::Input::GestureRecognizer^,
	_In_ Windows::UI::Input::ManipulationStartedEventArgs^)
{
}

void OutlookBackgroundCx::OnManipulationUpdated(
	_In_ Windows::UI::Input::GestureRecognizer^,
	_In_ Windows::UI::Input::ManipulationUpdatedEventArgs^ args)
{
	if (args->Delta.Expansion > 0) {
		_moveForwardTriggered = true;
	}

	if (args->Delta.Expansion < 0) {
		_moveBackTriggered = true;
	}

	if (args->Delta.Translation.X > 0) {
		_moveLeftTriggered = true;
	}

	if (args->Delta.Translation.X < 0) {
		_moveRightTriggered = true;
	}

}

void OutlookBackgroundCx::OnManipulationCompleted(
	_In_ Windows::UI::Input::GestureRecognizer^,
	_In_ Windows::UI::Input::ManipulationCompletedEventArgs^)
{
	_moveForwardTriggered = false;
	_moveBackTriggered = false;
	_moveRightTriggered = false;
	_moveLeftTriggered = false;

}

//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::Uninitialize
//
// This method is called during shutdown to disconnect the CoreApplicationView,
// and CoreWindow from the IFrameworkView.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::Uninitialize()
{
	_window = nullptr;
	_view = nullptr;
}


//------------------------------------------------------------------------------
//
// OutlookBackgroundCx::InitNewComposition
//
// This method is called by SetWindow(), where we initialize Composition after
// the CoreWindow has been created.
//
//------------------------------------------------------------------------------

void OutlookBackgroundCx::Exit()
{
	
}

void OutlookBackgroundCx::InitGraphics()
{
	_compGraphics = _compositor->DefaultGraphicsDevice;

	_materials[0] = _compGraphics->CreateImageFromUri(ref new Uri(L"ms-appx:///assets/roughceiling.jpg"));
	_materials[1] = _compGraphics->CreateImageFromUri(ref new Uri(L"ms-appx:///assets/concrete.jpg"));
	_materials[2] = _compGraphics->CreateImageFromUri(ref new Uri(L"ms-appx:///assets/brick1.jpg"));
	_materials[3] = _compGraphics->CreateImageFromUri(ref new Uri(L"ms-appx:///assets/wooden.jpg"));
	_materials[4] = _compGraphics->CreateImageFromUri(ref new Uri(L"ms-appx:///assets/lightmask.png"));
}


void OutlookBackgroundCx::InitNewComposition()
{
	//
	// Set up Windows.UI.Composition Compositor, root ContainerVisual, and associate with the
	// CoreWindow.
	//

	_compositor = ref new Compositor();

	_root = _compositor->CreateContainerVisual();
    _perspectiveVisual = _compositor->CreateContainerVisual();
    _cameraViewVisual = _compositor->CreateContainerVisual();

    _root->Children->InsertAtTop(_perspectiveVisual);
    _perspectiveVisual->Children->InsertAtTop(_cameraViewVisual);

	_target = _compositor->CreateTargetForCurrentView();
	_graphicsFactory = _compositor->DefaultGraphicsDevice;
	_target->Root = _root;

    CreateScene();

    for (int i = 0; i < _cTotalRooms; i++)
    {
        _roomRoots[i] = _compositor->CreateContainerVisual();
    }

//    _cameraViewVisual->Children->InsertAtTop(_sceneRoot);
//    _sceneRoot = _compositor->CreateContainerVisual();


    _cameraPos = float3(0.0f, -10000.0f, 0.0f);

    _moveLeftTriggered = false;
    _moveRightTriggered = false;

    _moveForwardTriggered = false;
    _moveBackTriggered = false;

    _turnLeftTriggered = false;
    _turnRightTriggered = false;

    _cameraAngle = 0.0f;

    _cameraLookAt.x = sinf(_cameraAngle);
    _cameraLookAt.y = cosf(_cameraAngle);
    _cameraLookAt.z = 0.0f;

    _cameraRight.x = sinf(_cameraAngle + DirectX::XM_PIDIV2);
    _cameraRight.y = cosf(_cameraAngle + DirectX::XM_PIDIV2);
    _cameraRight.z = 0.0f;

    _fPopulated = false;

	SetWalkAnimation();
}

void OutlookBackgroundCx::Render()
{
    float rightMovementMult = 0.0f;
    float forwardMovementMult = 0.0f;
    float turnRightMult = 0.0f;

    int iCurrentRoomId = 0;

    for (int iRoom = 0; iRoom < _cTotalRooms; iRoom++)
    {
        if (_totalRooms[iRoom]->WithinBounds(_cameraPos))
        {
            iCurrentRoomId = iRoom;
        }
    }

    if (_currentRoomId != iCurrentRoomId ||
        _currentRoomId == -1
        )
    {
        _cCurrentRooms = 0;

        _currentRoomId = iCurrentRoomId;

        _cameraViewVisual->Children->RemoveAll();

        for (int iAdjoiningRooms = 0; iAdjoiningRooms < _totalRooms[_currentRoomId]->GetNumAdjoiningRooms(); iAdjoiningRooms++)
        {
            int iAdjoiningId = _totalRooms[_currentRoomId]->GetAdjoiningRoomId(iAdjoiningRooms);

            _currentRooms[_cCurrentRooms] = _totalRooms[iAdjoiningId];
            _currentRoomRoots[_cCurrentRooms] = _roomRoots[iAdjoiningId];

            _cameraViewVisual->Children->InsertAtBottom(_roomRoots[iAdjoiningId]);

            _cCurrentRooms++;
        }

        _cameraViewVisual->Children->InsertAtTop(_roomRoots[_currentRoomId]);

        _currentRooms[_cCurrentRooms] = _totalRooms[_currentRoomId];
        _currentRoomRoots[_cCurrentRooms] = _roomRoots[_currentRoomId];

        _cCurrentRooms++;
    }

    if (_moveLeftTriggered != _moveRightTriggered)
    {
        if (_moveRightTriggered)
        {
            rightMovementMult = 1.0f;
        }
        else
        {
            rightMovementMult = -1.0f;
        }
    }

    if (_moveForwardTriggered != _moveBackTriggered)
    {
        if (_moveForwardTriggered)
        {
            forwardMovementMult = 1.0f;
        }
        else
        {
            forwardMovementMult = -1.0f;
        }
    }

	if (!_moveForwardTriggered &&
		!_moveBackTriggered &&
		!_moveLeftTriggered &&
		!_moveRightTriggered)
	{
		if (animationOn)
		{
			_cameraViewWalkAnimationX->Stop();
			_cameraViewWalkAnimationY->Stop();
			animationOn = false;
		}
	}
	else
	{
		_cameraViewWalkAnimationX->Start();
		_cameraViewWalkAnimationY->Start();
		animationOn = true;
	}

    if (_turnLeftTriggered != _turnRightTriggered)
    {
        if (_turnRightTriggered)
        {
            turnRightMult = 1.0f;
        }
        else
        {
            turnRightMult = -1.0f;
        }
    }

    _cameraAngle += DirectX::XM_PI*turnRightMult/100000.0f;

    _cameraLookAt.x = sinf(_cameraAngle);
    _cameraLookAt.y = cosf(_cameraAngle);
    _cameraLookAt.z = 0.0f;

    _cameraRight.x = sinf(_cameraAngle + DirectX::XM_PIDIV2);
    _cameraRight.y = cosf(_cameraAngle + DirectX::XM_PIDIV2);
    _cameraRight.z = 0.0f;

    float3 hitTestPos = _cameraPos + _cameraLookAt*forwardMovementMult*5000.0f + _cameraRight*rightMovementMult*5000.0f;
    bool fHit = false;

    if (_totalRooms[_currentRoomId]->WithinBounds(hitTestPos) ||
        _totalRooms[_currentRoomId]->GetEntry(_cameraPos) >= 0
        )
    {
        // doors are negative, walls >= 1
        if (_totalRooms[_currentRoomId]->GetEntry(hitTestPos) > 0            
            )
        {
            fHit = true;
        }
    }

    if (_totalRooms[_currentRoomId]->GetEntry(_cameraPos) == -1)
    {
        for (int iRoom = 0; iRoom < _cCurrentRooms; iRoom++)
        {
            _currentRooms[iRoom]->PlayDoor();
        }
    }

    if (!fHit)
    {
        _cameraPos += _cameraLookAt*forwardMovementMult;
        _cameraPos += _cameraRight*rightMovementMult;
    }

    _cameraViewVisual->TransformMatrix = make_float4x4_look_at(
        _cameraPos,
        _cameraPos + 1000.0f*_cameraLookAt,
        float3(0.0f, 0.0f, 1.0f)
        );
                
        // make_float4x4_translation(_cameraPos);

    float4x4 matPreOffset = make_float4x4_translation(
        -static_cast<float>(_window->Bounds.Width) / 2.0f,
        -static_cast<float>(_window->Bounds.Height) / 2.0f,
        0.0f
        );

    float4x4 matPerspective = float4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, -1.0f/800.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    float4x4 matPostOffset = make_float4x4_translation(
        static_cast<float>(_window->Bounds.Width) / 2.0f,
        static_cast<float>(_window->Bounds.Height) / 2.0f,
        0.0f
        );

    float4x4 matScale = make_float4x4_translation(
        1.0f,
        1.0f,
        1.0f
        );

    _perspectiveVisual->TransformMatrix = matScale * matPreOffset * matPerspective * matPostOffset;

    static bool forceRePopulate = false;

    bool lookXNonNegative = _cameraLookAt.x >= 0.0f;
    bool lookYNonNegative = _cameraLookAt.y >= 0.0f;
    bool lookXDominant = fabsf(_cameraLookAt.x) > fabsf(_cameraLookAt.y);

    if (lookXNonNegative != _curOrientationXNonNegative ||
        lookYNonNegative != _curOrientationYNonNegative ||
        lookXDominant != _curOrientationXDominant ||
        !_fPopulated ||
        forceRePopulate)
    {
        for (int i = 0; i < _cCurrentRooms; i++)
        {
            _currentRoomRoots[i]->Children->RemoveAll();
        }

        for (int i = 0; i < _cCurrentRooms; i++)
        {
            _currentRooms[i]->AppendVisuals(
            _compositor,
                _currentRoomRoots[i],
            _cameraLookAt,
			_materials
            );
        }

        _curOrientationXNonNegative = lookXNonNegative;
        _curOrientationYNonNegative = lookYNonNegative;
        _curOrientationXDominant = lookXDominant;

        _fPopulated = true;
    }
}

void 
OutlookBackgroundCx::AddRoom(
    __in GridRoom* pNewRoom
    )
{
    assert(_cTotalRooms < MAX_ROOMS);

    _totalRooms[_cTotalRooms] = pNewRoom;
    _cTotalRooms++;
}

void OutlookBackgroundCx::SetWalkAnimation()
{
	Windows::Foundation::TimeSpan timeSpan;
	timeSpan.Duration = 2000 * 10000;

	Windows::Foundation::Numerics::float3 offset = _cameraViewVisual->Offset;

	// Create the linear easing
	LinearEasingFunction^ linearEasing = _compositor->CreateLinearEasingFunction();

	// Create the x animation
	ScalarKeyFrameAnimation^ scalarAnimationX = _compositor->CreateScalarKeyFrameAnimation();

	// Create the y animation
	ScalarKeyFrameAnimation^ scalarAnimationY = _compositor->CreateScalarKeyFrameAnimation();

	// Create the poperty set
	CompositionPropertySet^ propSet = _compositor->CreatePropertySet();

	// Create the combining expression
	ExpressionAnimation^ expr = _compositor->CreateExpressionAnimation(L"Vector3(propSet.OffsetX, propSet.OffsetY, 0)");
	expr->SetReferenceParameter(L"propSet", propSet);

	// Set up the x animation
	scalarAnimationX->InsertKeyFrame(0.00f, offset.x, linearEasing);
	scalarAnimationX->InsertKeyFrame(0.23f, offset.x + 1000, linearEasing);
	scalarAnimationX->InsertKeyFrame(0.27f, offset.x + 1000, linearEasing);
	scalarAnimationX->InsertKeyFrame(0.50f, offset.x, linearEasing);
	scalarAnimationX->InsertKeyFrame(0.73f, offset.x - 1000, linearEasing);
	scalarAnimationX->InsertKeyFrame(0.77f, offset.x - 1000, linearEasing);
	scalarAnimationX->InsertKeyFrame(1.00f, offset.x, linearEasing);
	scalarAnimationX->Duration = timeSpan;
	scalarAnimationX->RepeatCount = -1;

	// Set up the y animation
	scalarAnimationY->InsertKeyFrame(0.00f, offset.y, linearEasing);
	scalarAnimationY->InsertKeyFrame(0.25f, offset.y - 1000.0f, linearEasing);
	scalarAnimationY->InsertKeyFrame(0.50f, offset.y, linearEasing);
	scalarAnimationY->InsertKeyFrame(0.75f, offset.y - 1000.0f, linearEasing);
	scalarAnimationY->InsertKeyFrame(1.00f, offset.y, linearEasing);
	scalarAnimationY->Duration = timeSpan;
	scalarAnimationY->RepeatCount = -1;

	// Set up the property set
	propSet->InsertScalar(L"OffsetX", offset.x);
	propSet->InsertScalar(L"OffsetY", offset.y);

	// Combine these animations using a property set
	CompositionPropertyAnimator^ animatorX = propSet->ConnectAnimation(L"OffsetX", scalarAnimationX);
	CompositionPropertyAnimator^ animatorY = propSet->ConnectAnimation(L"OffsetY", scalarAnimationY);
	//animatorX->Start();
	//animatorY->Start();

	// Bind the expression to the visual's offset
	CompositionPropertyAnimator^ animator = _cameraViewVisual->ConnectAnimation(L"Offset", expr);
	animator->Start();

	_cameraViewPropSet = propSet;
	_cameraViewWalkAnimation = animator;
	_cameraViewWalkAnimationX = animatorX;
	_cameraViewWalkAnimationY = animatorY;
}

void OutlookBackgroundCx::CreateScene()
{
    BasicRoom* pMainRoom = nullptr;
    BasicRoom* pTowerRoom = nullptr;

    // Main room
    {
    int entries[7][7];

    entries[0][0] = 0;
    entries[0][1] = 0;
    entries[0][2] = 0;
    entries[0][3] = -1;
    entries[0][4] = 0;
    entries[0][5] = 0;
    entries[0][6] = 0;

    entries[1][0] = 0;
    entries[1][1] = 0;
    entries[1][2] = 0;
    entries[1][3] = 0;
    entries[1][4] = 0;
    entries[1][5] = 0;
    entries[1][6] = 0;

    entries[2][0] = 0;
    entries[2][1] = 0;
    entries[2][2] = 2;
    entries[2][3] = 0;
    entries[2][4] = 3;
    entries[2][5] = 0;
    entries[2][6] = 0;

    entries[3][0] = 0;
    entries[3][1] = 0;
    entries[3][2] = 0;
    entries[3][3] = 0;
    entries[3][4] = 3;
    entries[3][5] = 0;
    entries[3][6] = 0;

    entries[4][0] = 0;
    entries[4][1] = 0;
    entries[4][2] = 0;
    entries[4][3] = 0;
    entries[4][4] = 3;
    entries[4][5] = 0;
    entries[4][6] = 0;

    entries[5][0] = 0;
    entries[5][1] = 0;
    entries[5][2] = 0;
    entries[5][3] = 0;
    entries[5][4] = 0;
    entries[5][5] = 0;
    entries[5][6] = 0;

    entries[6][0] = 0;
    entries[6][1] = 0;
    entries[6][2] = 0;
    entries[6][3] = 0;
    entries[6][4] = 0;
    entries[6][5] = 0;
    entries[6][6] = 0;

        pMainRoom = new BasicRoom;

        pMainRoom->Set(
        -2,
        -2,
        7,
            7,
            reinterpret_cast<int *>(entries)
            );

        AddRoom(pMainRoom);
    }

    // 2nd Room
    {
        int entries[14][7];

        entries[13][0] = 0;
        entries[13][1] = 0;
        entries[13][2] = 0;
        entries[13][3] = -1;
        entries[13][4] = 0;
        entries[13][5] = 0;
        entries[13][6] = 0;

        for (int iRow = 12; iRow >= 0; iRow-= 1)
        {
            entries[iRow][0] = 0;
            entries[iRow][1] = 0;
            entries[iRow][2] = 0;
            entries[iRow][3] = 0;
            entries[iRow][4] = 0;
            entries[iRow][5] = 0;
            entries[iRow][6] = 0;

            if (iRow % 4 == 1)
            {
                entries[iRow][2] = 3;
                entries[iRow][4] = 3;
            }

            if (iRow % 4 == 3)
            {
                entries[iRow][2] = 2;
                entries[iRow][4] = 2;
            }
        }

        pTowerRoom = new BasicRoom;

        pTowerRoom->Set(
            -2,
            -2 - 14,
        7,
            14,
        reinterpret_cast<int *>(entries)
        );

        AddRoom(pTowerRoom);
    }

    pMainRoom->AddAdjoiningRoom(pTowerRoom, 1);
    pTowerRoom->AddAdjoiningRoom(pMainRoom, 0);
}

/*
void OutlookBackgroundCx::ApplySaturationEffect()
{
	SolidColorVisual ^ bg = _compositor->CreateSolidColorVisual();
	bg->Color = Windows::UI::Colors::CornflowerBlue;
	bg->Size = float2(1200.0f, 600.0f);
	bg->Offset = float3(50.0f, 50.0f, 0.0f);
	_sceneRoot->Children->InsertAtTop(bg);

	
	CompositionImage^ testImage = _graphicsFactory->CreateImageFromUri(ref new Uri(L"http://www.hdwallpapersimages.com/wp-content/uploads/2015/02/Adorable-Bolognese-Dog-Images-1024x681.jpg"));

	auto imgVisual = _compositor->CreateImageVisual();
	imgVisual->Content = testImage;
	imgVisual->Size = float2(450.0f, 450.0f);
	imgVisual->Offset = float3(50.0f, 50.0f, 0.0f);
	imgVisual->StretchPolicy = CompositionStretchPolicy::Fill;
	bg->Children->InsertAtTop(imgVisual);

	auto effectDesc = ref new SaturationEffect();
	effectDesc->Saturation = 0;
	effectDesc->Source = ref new CompositionEffectNamedInput("Image");

	auto effectTemplate = _compositor->CreateEffectTemplate((Windows::Graphics::Effects::IGraphicsEffect^)effectDesc);

	auto effect = effectTemplate->CreateInstance();
	
	auto effectVisual = _compositor->CreateEffectVisual();
	effectVisual->Effect = effect;
	effectVisual->Size = float2(450.0f, 450.0f);
	effectVisual->Offset = float3(550.0f, 50.0f, 0.0f);
	bg->Children->InsertAtTop(effectVisual);

	testImage->TrackLoadAsync()->Completed = ref new AsyncActionWithProgressCompletedHandler<UINT>([testImage, effect](IAsyncActionWithProgress<UINT> ^ loadAction, AsyncStatus ) {
		effect->SetInput(L"Image", testImage);
	});
}


void OutlookBackgroundCx::ApplyArithematicCompositeEffect()
{
	SolidColorVisual ^ bg = _compositor->CreateSolidColorVisual();
	bg->Color = Windows::UI::Colors::CornflowerBlue;
	bg->Size = float2(1200.0f, 600.0f);
	bg->Offset = float3(50.0f, 50.0f, 0.0f);
	_sceneRoot->Children->InsertAtTop(bg);

	CompositionImage^ testImage1 = _graphicsFactory->CreateImageFromUri(ref new Uri(L"http://www.hdwallpapersimages.com/wp-content/uploads/2015/02/Field-Of-Daisies-Sun-Rays-Light-Images.jpg"));
	CompositionImage^ testImage2 = _graphicsFactory->CreateImageFromUri(ref new Uri(L"http://www.hdwallpapersimages.com/wp-content/uploads/2015/02/Fresh-Daisy-Flower-Images.jpg"));

	auto imgVisual1 = _compositor->CreateImageVisual();
	imgVisual1->Content = testImage1;
	imgVisual1->Size = float2(250.0f, 250.0f);
	imgVisual1->Offset = float3(50.0f, 50.0f, 0.0f);
	imgVisual1->StretchPolicy = CompositionStretchPolicy::Fill;
	bg->Children->InsertAtTop(imgVisual1);

	auto imgVisual2 = _compositor->CreateImageVisual();
	imgVisual2->Content = testImage2;
	imgVisual2->Size = float2(250.0f, 250.0f);
	imgVisual2->Offset = float3(350.0f, 50.0f, 0.0f);
	imgVisual2->StretchPolicy = CompositionStretchPolicy::Fill;
	bg->Children->InsertAtTop(imgVisual2);

	
	
	auto effectDesc = ref new ArithmeticCompositeEffect();
	effectDesc->Name = "MyCrossfade";
	effectDesc->MultiplyAmount = 0.0f;
	effectDesc->Offset = 0.0f;
	effectDesc->Source1Amount = 0.5f;
	effectDesc->Source2Amount = 0.5f;
	effectDesc->Source1 = ref new CompositionEffectNamedInput("MyInput1");
	effectDesc->Source2 = ref new CompositionEffectNamedInput("MyInput2");
	auto effectTemplate = _compositor->CreateEffectTemplate((Windows::Graphics::Effects::IGraphicsEffect^)effectDesc);
	auto effect = effectTemplate->CreateInstance();

	auto effectVisual = _compositor->CreateEffectVisual();
	effectVisual->Effect = effect;
	effectVisual->Size = float2(250.0f, 250.0f);
	effectVisual->Offset = float3(650.0f, 50.0f, 0.0f);
	bg->Children->InsertAtTop(effectVisual);

	testImage1->TrackLoadAsync()->Completed = ref new AsyncActionWithProgressCompletedHandler<UINT>([testImage1, effect](IAsyncActionWithProgress<UINT> ^ loadAction, AsyncStatus ) {
		effect->SetInput(L"MyInput1", testImage1);
	});

	testImage2->TrackLoadAsync()->Completed = ref new AsyncActionWithProgressCompletedHandler<UINT>([testImage2, effect](IAsyncActionWithProgress<UINT> ^ loadAction, AsyncStatus ) {
		effect->SetInput(L"MyInput2", testImage2);
	});

}

void OutlookBackgroundCx::ApplyArithematicCompositeEffectWithAnimation()
{
	SolidColorVisual ^ bg = _compositor->CreateSolidColorVisual();
	bg->Color = Windows::UI::Colors::CornflowerBlue;
	bg->Size = float2(1200.0f, 600.0f);
	bg->Offset = float3(50.0f, 50.0f, 0.0f);
	_sceneRoot->Children->InsertAtTop(bg);

	CompositionImage^ testImage1 = _graphicsFactory->CreateImageFromUri(ref new Uri(L"http://www.hdwallpapersimages.com/wp-content/uploads/2015/02/Field-Of-Daisies-Sun-Rays-Light-Images.jpg"));
	CompositionImage^ testImage2 = _graphicsFactory->CreateImageFromUri(ref new Uri(L"http://www.hdwallpapersimages.com/wp-content/uploads/2015/02/Fresh-Daisy-Flower-Images.jpg"));

	auto imgVisual1 = _compositor->CreateImageVisual();
	imgVisual1->Content = testImage1;
	imgVisual1->Size = float2(250.0f, 250.0f);
	imgVisual1->Offset = float3(50.0f, 50.0f, 0.0f);
	imgVisual1->StretchPolicy = CompositionStretchPolicy::Fill;
	bg->Children->InsertAtTop(imgVisual1);

	auto imgVisual2 = _compositor->CreateImageVisual();
	imgVisual2->Content = testImage2;
	imgVisual2->Size = float2(250.0f, 250.0f);
	imgVisual2->Offset = float3(350.0f, 50.0f, 0.0f);
	imgVisual2->StretchPolicy = CompositionStretchPolicy::Fill;
	bg->Children->InsertAtTop(imgVisual2);



	auto effectDesc = ref new ArithmeticCompositeEffect();
	effectDesc->Name = "MyCrossfade";
	effectDesc->MultiplyAmount = 0.0f;
	effectDesc->Offset = 0.0f;
	effectDesc->Source1Amount = 0.5f;
	effectDesc->Source2Amount = 0.5f;
	effectDesc->Source1 = ref new CompositionEffectNamedInput("MyInput1");
	effectDesc->Source2 = ref new CompositionEffectNamedInput("MyInput2");

	Platform::Collections::Vector<String^>^ animatableProperties = ref new Platform::Collections::Vector<String^>();
	animatableProperties->Append("MyCrossfade.Source1Amount");
	animatableProperties->Append("MyCrossfade.Source2Amount");

	auto easeLinear = _compositor->CreateLinearEasingFunction();
	auto blendAnimationSource1 = _compositor->CreateScalarKeyFrameAnimation();	
	blendAnimationSource1->InsertKeyFrame(0.00f, 0.00f, easeLinear);
	blendAnimationSource1->InsertKeyFrame(1.00f, 1.00f, easeLinear);

	auto blendAnimationSource2 = _compositor->CreateScalarKeyFrameAnimation();
	blendAnimationSource2->InsertKeyFrame(0.00f, 1.00f, easeLinear);
	blendAnimationSource2->InsertKeyFrame(1.00f, 0.00f, easeLinear);

	TimeSpan duration = { 10 * 10000000 };
	blendAnimationSource1->Duration = duration;
	blendAnimationSource1->RepeatCount = -1;
	blendAnimationSource2->Duration = duration;
	blendAnimationSource2->RepeatCount = -1;

	auto effectTemplate = _compositor->CreateEffectTemplate((Windows::Graphics::Effects::IGraphicsEffect^)effectDesc, animatableProperties);
	auto effect = effectTemplate->CreateInstance();

	effect->Properties->ConnectAnimation("MyCrossfade.Source1Amount", blendAnimationSource1);
	effect->Properties->ConnectAnimation("MyCrossfade.Source2Amount", blendAnimationSource2);

	auto effectVisual = _compositor->CreateEffectVisual();
	effectVisual->Effect = effect;
	effectVisual->Size = float2(250.0f, 250.0f);
	effectVisual->Offset = float3(650.0f, 50.0f, 0.0f);
	bg->Children->InsertAtTop(effectVisual);

	testImage1->TrackLoadAsync()->Completed = ref new AsyncActionWithProgressCompletedHandler<UINT>([testImage1, effect](IAsyncActionWithProgress<UINT> ^ loadAction, AsyncStatus ) {
		effect->SetInput(L"MyInput1", testImage1);
	});

	testImage2->TrackLoadAsync()->Completed = ref new AsyncActionWithProgressCompletedHandler<UINT>([testImage2, effect](IAsyncActionWithProgress<UINT> ^ loadAction, AsyncStatus ) {
		effect->SetInput(L"MyInput2", testImage2);
	});

}
*/

//------------------------------------------------------------------------------
//
// OutlookBackgroundCxFactory::CreateView
//
// This method is called by CoreApplication to provide a new IFrameworkView for
// a CoreWindow that is being created.
//
//------------------------------------------------------------------------------

IFrameworkView ^ OutlookBackgroundCxFactory::CreateView()
{
	return ref new OutlookBackgroundCx();
}


//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------

int __cdecl main(Platform::Array<String ^> ^ args)
{
	CoreApplication::Run(ref new OutlookBackgroundCxFactory());

	return 0;
}
