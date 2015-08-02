//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

#include "DolphinRenderer.h"

ref class DolphinCppCx sealed : public IFrameworkView
{
public:
    // IFrameworkView
    virtual void Initialize(
        _In_ CoreApplicationView ^ view);

    virtual void SetWindow(
        _In_ CoreWindow ^ window);

    virtual void Load(
        _In_ String ^ string);

    virtual void Run();

    virtual void Uninitialize();

    // Event Handlers
    void OnActivated(
        _In_ CoreApplicationView ^ view,
        _In_ IActivatedEventArgs ^ eventArgs);

    void OnVisibilityChanged(
        _In_ Windows::UI::Core::CoreWindow ^ sender, 
        _In_ Windows::UI::Core::VisibilityChangedEventArgs ^ eventArgs);

    void OnWindowClosed(
        _In_ Windows::UI::Core::CoreWindow ^ sender,
        _In_ Windows::UI::Core::CoreWindowEventArgs ^ eventArgs);

private:
    void InitializeComposition(
        _In_ CoreWindow ^ window);

private:
    DolphinRenderer                 _renderer;    

    // Windows.UI.Composition
    Agile<CoreApplicationView>      _view;
    Compositor ^                    _compositor;
    CompositionTarget ^             _compositionTarget;
    ContainerVisual ^               _root;

    // Window state
    boolean                         _windowClosed;
    boolean                         _windowVisible;
};


ref class DolphinCppCxFactory sealed : IFrameworkViewSource
{
public:
    virtual IFrameworkView ^ CreateView();
};
