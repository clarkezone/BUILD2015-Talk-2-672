//------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once


ref class BasicTimer sealed
{
public:
    BasicTimer();

public:
    void Reset();

    void Update();

    property float Total
    {
        float get() { return m_total; }
    }

    property float Delta
    {
        float get() { return m_delta; }
    }

private:
    LARGE_INTEGER   m_frequency;
    LARGE_INTEGER   m_currentTime;
    LARGE_INTEGER   m_startTime;
    LARGE_INTEGER   m_lastTime;
    float           m_total;
    float           m_delta;
};
