// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using System;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace ExampleGallery
{
    public sealed class BurningTextExample
    {

        // References to specific effects so we can dynamically update their properties.
        CanvasCommandList textCommandList;
        MorphologyEffect morphology;
        CompositeEffect composite;
        Transform2DEffect flameAnimation;
        Transform2DEffect flamePosition;

        public void CreateResources()
        {

            CreateFlameEffect();
        }

        //int count;

        public void Draw(CanvasDrawingSession ds, Size sizeRender, float totalTimeSeconds)
        {
            //count++;
            //ds.DrawText(count.ToString(), new Vector2(10.0f, 10.0f), Colors.Red);
            SetupText(ds);
            ConfigureEffect(totalTimeSeconds);

            ds.DrawImage(composite, sizeRender.ToVector2() / 2);
        }

        private void ConfigureEffect(float totalTimeSeconds)
        {
            // Animate the flame by shifting the Perlin noise upwards (-Y) over time.
            flameAnimation.TransformMatrix = Matrix3x2.CreateTranslation(0, -totalTimeSeconds * 60.0f);

            // Scale the flame effect 2x vertically, aligned so it starts above the text.
            float verticalOffset = /*fontSize*/ 20 * 1.4f;

            var centerPoint = new Vector2(0, verticalOffset);

            flamePosition.TransformMatrix = Matrix3x2.CreateScale(1, 2, centerPoint);
        }

        /// <summary>
        /// Renders text into a command list and sets this as the input to the flame
        /// effect graph. The effect graph must already be created before calling this method.
        /// </summary>
        private void SetupText(ICanvasResourceCreator resourceCreator)
        {
            textCommandList = new CanvasCommandList(resourceCreator);

            using (var ds = textCommandList.CreateDrawingSession())
            {
                ds.Clear(Colors.Transparent);

                ds.DrawText(
                    "Windows.UI.Composition",
                    0,
                    0,
                    Colors.White,
                    new Microsoft.Graphics.Canvas.Text.CanvasTextFormat
                    {
                        FontFamily = "Segoe UI",
                        FontSize = 20,
                        HorizontalAlignment = CanvasHorizontalAlignment.Center,
                        VerticalAlignment = CanvasVerticalAlignment.Top
                    });
            }

            // Hook up the command list to the inputs of the flame effect graph.
            morphology.Source = textCommandList;
            composite.Sources[1] = textCommandList;
        }

        /// <summary>
        /// Calculates a good font size so the text will fit even on smaller phone screens.
        /// </summary>
        private static float GetFontSize(Size displaySize)
        {
            const float maxFontSize = 72;
            const float scaleFactor = 12;

            return Math.Min((float)displaySize.Width / scaleFactor, maxFontSize);
        }

        /// <summary>
        /// Generate the flame effect graph. This method is called before the text command list
        /// (input) is created.
        /// </summary>
        private void CreateFlameEffect()
        {
            // Thicken the text.
            morphology = new MorphologyEffect
            {
                // The Source property is set by SetupText().
                Mode = MorphologyEffectMode.Dilate,
                Width = 7,
                Height = 1
            };

            // Blur, then colorize the text from black to red to orange as the alpha increases.
            var colorize = new ColorMatrixEffect
            {
                Source = new GaussianBlurEffect
                {
                    Source = morphology,
                    BlurAmount = 3f
                },
                ColorMatrix = new Matrix5x4
                {
                    M11 = 0f, M12 = 0f, M13 = 0f, M14 = 0f,
                    M21 = 0f, M22 = 0f, M23 = 0f, M24 = 0f,
                    M31 = 0f, M32 = 0f, M33 = 0f, M34 = 0f,
                    M41 = 0f, M42 = 1f, M43 = 0f, M44 = 1f,
                    M51 = 1f, M52 = -0.5f, M53 = 0f, M54 = 0f
                }
            };

            // Generate a Perlin noise field (see flamePosition).
            // Animate the noise by modifying flameAnimation's transform matrix at render time.
            flameAnimation = new Transform2DEffect
            {
                Source = new BorderEffect
                {
                    Source = new TurbulenceEffect
                    {
                        Frequency = new Vector2(0.109f, 0.109f),
                        Size = new Vector2(500.0f, 80.0f)
                    },
                    // Use Mirror extend mode to allow us to spatially translate the noise
                    // without any visible seams.
                    ExtendX = CanvasEdgeBehavior.Mirror,
                    ExtendY = CanvasEdgeBehavior.Mirror
                }
            };

            // Give the flame its wavy appearance by generating a displacement map from the noise
            // (see flameAnimation) and applying this to the text.
            // Stretch and position this flame behind the original text.
            flamePosition = new Transform2DEffect
            {
                Source = new DisplacementMapEffect
                {
                    Source = colorize,
                    Displacement = flameAnimation,
                    Amount = 40.0f
                }
                // Set the transform matrix at render time as it depends on window size.
            };

            // Composite the text over the flames.
            composite = new CompositeEffect()
            {
                Sources = { flamePosition, null }
                // Replace null with the text command list when it is created.
            };
        }
    }
}
