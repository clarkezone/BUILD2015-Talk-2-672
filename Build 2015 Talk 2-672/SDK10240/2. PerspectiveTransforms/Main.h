//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
//
// OutlookBackgroundCx
//
// A prototype of the Outlook background image scenario.
//
//------------------------------------------------------------------------------

ref class OutlookBackgroundCx : public IFrameworkView
{
public:
	// IFrameworkView
	virtual void Initialize(CoreApplicationView ^ view);

	virtual void SetWindow(CoreWindow ^ window);

	virtual void Load(String ^ string);

	virtual void Run();

	virtual void Uninitialize();

	virtual void Exit();

    void OnKeyDown(
        __in CoreWindow^ pSender,
        __in KeyEventArgs^ e
        );

    void OnKeyUp(
        __in CoreWindow^ pSender,
        __in KeyEventArgs^ e
        );
	void SetDevice(_In_opt_ Windows::Devices::Input::IPointerDevice^ pointerDevice);

	void OnManipulationStarted(
		_In_ Windows::UI::Input::GestureRecognizer^ sender,
		_In_ Windows::UI::Input::ManipulationStartedEventArgs^ args);
	void OnManipulationUpdated(
		_In_ Windows::UI::Input::GestureRecognizer^ sender,
		_In_ Windows::UI::Input::ManipulationUpdatedEventArgs^ args);
	void OnManipulationCompleted(
		_In_ Windows::UI::Input::GestureRecognizer^ sender,
		_In_ Windows::UI::Input::ManipulationCompletedEventArgs^ args);
	void OnPointerPressed(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerReleased(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerMoved(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args);

private:
	void InitNewComposition();
	void InitGraphics();
	void ApplySaturationEffect();
	void ApplyArithematicCompositeEffect();
	void ApplyArithematicCompositeEffectWithAnimation();

    void CreateScene();

    void Render();

	void SetWalkAnimation();

    void AddRoom(
        __in GridRoom* pNewRoom
        );

private:
	// CoreWindow / CoreApplicationView
	Agile<CoreWindow> _window;
	Agile<CoreApplicationView> _view;

	// Windows.UI.Composition
	Compositor ^ _compositor;
	CompositionGraphicsDevice^ _graphicsFactory;
	CompositionTarget ^ _target;
	ContainerVisual^ _root;
    ContainerVisual^ _perspectiveVisual;
    ContainerVisual^ _cameraViewVisual;

    ContainerVisual^ _currentRoomRoots[MAX_ROOMS];
    GridRoom* _currentRooms[MAX_ROOMS];
    int _cCurrentRooms;

    int _currentRoomId;

    bool _windowClosed;
    bool _windowVisible;

    float _cameraAngle;

    float3 _cameraPos;
    float3 _cameraLookAt;
    float3 _cameraRight;
    float3 _cameraUp;

    bool _moveLeftTriggered;
    bool _moveRightTriggered;
    bool _moveForwardTriggered;
    bool _moveBackTriggered;
    bool _turnLeftTriggered;
    bool _turnRightTriggered;

//    GridRoom* _pCurrentRoom;

    bool _curOrientationXNonNegative;
    bool _curOrientationYNonNegative;
    bool _curOrientationXDominant;
    bool _fPopulated;

	ICompositionGraphicsDevice^ _compGraphics;
	ICompositionSurface^ _materials[5];
	CompositionPropertySet^ _cameraViewPropSet;
	bool animationOn = false;
	CompositionPropertyAnimator^ _cameraViewWalkAnimation;
	CompositionPropertyAnimator^ _cameraViewWalkAnimationX;
	CompositionPropertyAnimator^ _cameraViewWalkAnimationY;

	Windows::Devices::Input::IPointerDevice^ _pointerDevice;
	Windows::UI::Input::GestureRecognizer^ _gestureRecognizer;
	Windows::Foundation::EventRegistrationToken _tokenManipulationStarted;
	Windows::Foundation::EventRegistrationToken _tokenManipulationUpdated;
	Windows::Foundation::EventRegistrationToken _tokenManipulationCompleted;

    GridRoom* _totalRooms[MAX_ROOMS];
    ContainerVisual^ _roomRoots[MAX_ROOMS];

    int _cTotalRooms;
};


ref class OutlookBackgroundCxFactory : IFrameworkViewSource
{
public:
	virtual IFrameworkView ^ CreateView();
};
