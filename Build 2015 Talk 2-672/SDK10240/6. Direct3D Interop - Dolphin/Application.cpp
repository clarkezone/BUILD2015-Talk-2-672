//  Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"
#include "Application.h"
#include "BasicTimer.h"


//------------------------------------------------------------------------------
//
// DolphinCppCx::Initialize
//
// This method is called during startup to associate the IFrameworkView with the
// CoreApplicationView.
//
//------------------------------------------------------------------------------

void DolphinCppCx::Initialize(CoreApplicationView ^ view)
{
    _view = view;

    //
    // Subscribe to events
    //

    view->Activated += ref new TypedEventHandler<CoreApplicationView ^, IActivatedEventArgs ^>(
            this, 
            &DolphinCppCx::OnActivated); 
}


//------------------------------------------------------------------------------
//
// DolphinCppCx::SetWindow
//
// This method is called when the CoreApplication has created a new CoreWindow,
// allowing the application to configure the window and start producing content
// to display.
//
//------------------------------------------------------------------------------

void DolphinCppCx::SetWindow(CoreWindow ^ window)
{
    // 
    // Setup our resources
    //

    Windows::Foundation::Size size = { 800, 600 };
    _renderer.Initialize(size);
    InitializeComposition(window);

    //
    // Subscribe to events
    //

    window->VisibilityChanged += ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(
            this, 
            &DolphinCppCx::OnVisibilityChanged);

    window->Closed += ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(
            this, 
            &DolphinCppCx::OnWindowClosed);
}


//------------------------------------------------------------------------------
//
// DolphinCppCx::Load
//
// This method is called when a specific page is being loaded in the
// application.  It is not used for this application.
//
//------------------------------------------------------------------------------

void DolphinCppCx::Load(String ^)
{

}


//------------------------------------------------------------------------------
//
// DolphinCppCx::Run
//
// This method is called by CoreApplication::Run() to actually run the
// dispatcher's message pump.
//
//------------------------------------------------------------------------------

void DolphinCppCx::Run()
{
    BasicTimer^ timer = ref new BasicTimer();

    while (!_windowClosed)
    {
        if (_windowVisible)
        {
            timer->Update();
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            _renderer.Update(timer->Total, timer->Delta);
            _renderer.Render();
            _renderer.Present();
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}


//------------------------------------------------------------------------------
//
// DolphinCppCx::Uninitialize
//
// This method is called during shutdown to disconnect the CoreApplicationView,
// and CoreWindow from the IFrameworkView.
//
//------------------------------------------------------------------------------

void DolphinCppCx::Uninitialize()
{
    _view = nullptr;
    _compositor = nullptr;
    _root = nullptr;
}


//------------------------------------------------------------------------------
//
// DolphinCppCx::OnActivated
//
// Handler for view activation event
//
//------------------------------------------------------------------------------

void DolphinCppCx::OnActivated(
    _In_ CoreApplicationView ^ view,
    _In_ IActivatedEventArgs ^ eventArgs)
{
    CoreWindow::GetForCurrentThread()->Activate();
}

//------------------------------------------------------------------------------
//
// DolphinCppCx::OnVisibilityChanged
//
// Handler for window visibility change events
//
//------------------------------------------------------------------------------

void DolphinCppCx::OnVisibilityChanged(
    _In_ Windows::UI::Core::CoreWindow ^ sender,
    _In_ Windows::UI::Core::VisibilityChangedEventArgs ^ eventArgs)
{
    _windowVisible = eventArgs->Visible;
}


//------------------------------------------------------------------------------
//
// DolphinCppCx::OnWindowClosed
//
// Handler for window close event
//
//------------------------------------------------------------------------------

void DolphinCppCx::OnWindowClosed(
    _In_ Windows::UI::Core::CoreWindow ^ sender,
    _In_ Windows::UI::Core::CoreWindowEventArgs ^ eventArgs)
{
    _windowClosed = true;
}

//------------------------------------------------------------------------------
//
// DolphinCppCx::OnWindowClosed
//
// Handler for window close event
//
//------------------------------------------------------------------------------

void DolphinCppCx::InitializeComposition(
    _In_ CoreWindow ^ window)
{
    //
    // Set up Windows.UI.Composition Compositor, root ContainerVisual, and associate with the
    // CoreWindow.
    //

    _compositor = ref new Compositor();

    _root = _compositor->CreateContainerVisual();

    _compositionTarget = _compositor->CreateTargetForCurrentView();
    _compositionTarget->Root = _root;

	_root->Scale = float3(1.8, 1.8, 0);

    //
    // Create a simple scene.
    //

    auto solidColorVisual = _compositor->CreateSolidColorVisual();
	solidColorVisual->Offset = float3(310.0f, 210.0f, 0.0f); 
    solidColorVisual->Size = float2(100.0f, 100.0f);
    solidColorVisual->Color = ColorHelper::FromArgb(0xFF, 0x00, 0xAA, 0xEE);
    _root->Children->InsertAtTop(solidColorVisual);

    
    auto imageVisual = _compositor->CreateImageVisual();
    imageVisual->Offset = float3(solidColorVisual->Offset.x - 300, solidColorVisual->Offset.y - 200, 0.0f);
    imageVisual->Size = float2(300, 200);
    _root->Children->InsertAtBottom(imageVisual);

    //
    // Create a CompositionSurface wrapper for our swapchain and then set as content
    // on our ImageVisual. To do this we need to drop down into standard C++
    //

	Microsoft::WRL::ComPtr<ABI::Windows::UI::Composition::ICompositorInterop> compositorNative = nullptr;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Composition::ICompositionSurface> compositionSurface = nullptr;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Composition::IImageVisual> imagevisualNative = nullptr;

    Microsoft::WRL::ComPtr<IInspectable> compositorUnderlyingType = reinterpret_cast<IInspectable*>(_compositor);
    Microsoft::WRL::ComPtr<IInspectable> visualUnderlyingType = reinterpret_cast<IInspectable*>(imageVisual);
    ThrowIfFailed(compositorUnderlyingType.As(&compositorNative));
    ThrowIfFailed(visualUnderlyingType.As(&imagevisualNative));

    ThrowIfFailed(compositorNative->CreateCompositionSurfaceForSwapChain(
        _renderer.GetSwapChain(),
        compositionSurface.GetAddressOf()));

    imagevisualNative->put_Image(compositionSurface.Get());

    //
    // Animate our scene
    //

    auto offsetAnimation = _compositor->CreateVector3KeyFrameAnimation();
    auto linearEasing = _compositor->CreateLinearEasingFunction();

    offsetAnimation->InsertKeyFrame(
        0.00f, 
        float3(imageVisual->Offset.x, imageVisual->Offset.y, 0.0f),
        linearEasing);
    offsetAnimation->InsertKeyFrame(
        0.25f, 
        float3(imageVisual->Offset.x + imageVisual->Size.x + solidColorVisual->Size.x, imageVisual->Offset.y, 0.0f),
        linearEasing);
    offsetAnimation->InsertKeyFrame(
        0.50f, 
        float3(imageVisual->Offset.x + imageVisual->Size.x + solidColorVisual->Size.x, imageVisual->Offset.y + imageVisual->Size.y + solidColorVisual->Size.y, 0.0f),
        linearEasing);
    offsetAnimation->InsertKeyFrame(
        0.75f, 
        float3(imageVisual->Offset.x, imageVisual->Offset.y + imageVisual->Size.y + solidColorVisual->Size.y, 0.0f),
        linearEasing);
    offsetAnimation->InsertKeyFrame(
        1.00f, 
        float3(imageVisual->Offset.x, imageVisual->Offset.y, 0.0f),
        linearEasing);

    TimeSpan duration = { 20 * 10000000 };
    offsetAnimation->Duration = duration;
    offsetAnimation->IterationCount = -1;

    imageVisual->ConnectAnimation(L"Offset", offsetAnimation)->Start();
}


//------------------------------------------------------------------------------
//
// DolphinCppCxFactory::CreateView
//
// This method is called by CoreApplication to provide a new IFrameworkView for
// a CoreWindow that is being created.
//
//------------------------------------------------------------------------------

IFrameworkView ^ DolphinCppCxFactory::CreateView()
{
    return ref new DolphinCppCx();
}


//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------

int __cdecl main(Platform::Array<String ^> ^ args)
{
    CoreApplication::Run(ref new DolphinCppCxFactory());

    return 0;
}
