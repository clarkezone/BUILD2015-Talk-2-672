using Microsoft.Graphics.Canvas.Effects;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace _5.Composition_Animation
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        Compositor _compositor;
        ContainerVisual _rootVisual;

        public MainPage()
        {
            this.InitializeComponent();
            this.Loaded += MainPage_Loaded;
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            InitializeCompositior();
            BuildVisualTree();
        }

        private void InitializeCompositior()
        {
            _rootVisual = ElementCompositionPreview.GetContainerVisual(followersHost) as ContainerVisual;
            _compositor = _rootVisual.Compositor;
        }

        private void BuildVisualTree()
        {
            // use current position for laying out as we iterate
            Vector3 currentGridPosition = new Vector3(horizontalSpace, horizontalSpace, 1.0f);
            Vector2 imageSize = new Vector2(imageDimension, imageDimension);

            Vector3KeyFrameAnimation firstAnimation = _compositor.CreateVector3KeyFrameAnimation();
            firstAnimation.InsertKeyFrame(0.0f, new Vector3(-50.0f, -50.0f, 0.0f));
            firstAnimation.InsertKeyFrame(1.0f, currentGridPosition);
            firstAnimation.Duration = TimeSpan.FromMilliseconds(2000);

            var animation = _compositor.CreateVector3KeyFrameAnimation();
            animation.InsertExpressionKeyFrame(0.0f, "previous.Offset");
            animation.InsertExpressionKeyFrame(1.0f, "finalvalue");
            animation.DelayTime = TimeSpan.FromMilliseconds(1000);
            animation.Duration = TimeSpan.FromMilliseconds(1500);

            Visual previousVisual = null;

            foreach (Uri profileImageUri in GetProfileUrisForTwitterFollowers("@wincomposition"))
            {
                var profileImageVisual = MakeImageVisualforFollower(profileImageUri, imageSize);

                if (previousVisual == null)
                {
                    firstAnimation.SetVector3Parameter("finalvalue", currentGridPosition);
                    profileImageVisual.ConnectAnimation("Offset", firstAnimation).Start();
                }
                else
                {
                    animation.SetReferenceParameter("previous", previousVisual);
                    animation.SetVector3Parameter("finalvalue", currentGridPosition);
                    animation.DelayTime += TimeSpan.FromMilliseconds(300); //50
                    profileImageVisual.ConnectAnimation("Offset", animation).Start();
                }

                _rootVisual.Children.InsertAtBottom(profileImageVisual);

                previousVisual = profileImageVisual;

                #region CalcNextOffset
                // simple dumb layout
                currentGridPosition.X += horizontalSpace + imageDimension;

                if (currentGridPosition.X + imageDimension > this.ActualWidth)
                {
                    currentGridPosition.X = horizontalSpace;
                    currentGridPosition.Y += imageDimension + verticalSpace;
                }
                #endregion
            }
        }

        private Visual MakeImageVisualforFollower(Uri imageUri, Vector2 size)
        {
            //Load profile image into a CompositionImage
            CompositionImage profileImage =
                _compositor.DefaultGraphicsDevice.CreateImageFromUri(imageUri);

            CompositionImage maskImage = _compositor.DefaultGraphicsDevice.CreateImageFromUri(new Uri("ms-appx:///Assets/CircularAlphaMaskInflated.png"));

            CompositeEffect maskComposite = new Microsoft.Graphics.Canvas.Effects.CompositeEffect();
            maskComposite.Sources.Add(new CompositionEffectSourceParameter("image"));
            maskComposite.Sources.Add(new CompositionEffectSourceParameter("maskImage"));
            maskComposite.Mode = Microsoft.Graphics.Canvas.CanvasComposite.DestinationIn;
            maskComposite.Name = "Mask";

            CompositionEffectFactory effectFactory = _compositor.CreateEffectFactory(maskComposite);

            CompositionEffect maskEffect = effectFactory.CreateEffect();
            maskEffect.SetSourceParameter("image", profileImage);
            maskEffect.SetSourceParameter("maskImage", maskImage);

            EffectVisual profileImageVisual = _compositor.CreateEffectVisual();
            profileImageVisual.Effect = maskEffect;
            profileImageVisual.Size = size;

            return profileImageVisual;
        }


        private IEnumerable<Uri> GetProfileUrisForTwitterFollowers(string user)
        {
            // use an offline collection of our first 30 followers

            List<Uri> tweets = new List<Uri>();
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/Tulip_Festival_2009_042_resized_400x400.JPG"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/0a29dc6e84880b6e284c5be6225060a6_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/Avatar4_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/SG_icon_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/default_profile_0_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/twitterProfilePhoto_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/kpe_C6-S_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/00859_radioactive_1920x1200_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/bVANF0zf_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/l182mF-a_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/JBx0SF56_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/XBox_Avatar_Picture_400x400.JPG"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/1db67d07beda02f117a36a79a1132a77_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/cOirIQsc_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/MwsXRNje_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/a60236af1fe6d45f29e1c8fcd7405b58_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/JtKiqOQs_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/mary_head_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/14cf791c3db09cfb265c93bcc499a4e4_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/BNxXzC8b_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/QWqo-iAm_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/eBWSe3Sy_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/87YCdMc8_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/Su_0dynR_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/a3dd95a57f18917ac3541ef779c281b4_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/mEnhIdha_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/_KClakp8_400x400.png"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/gVNFuhOX_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/sHDYQSYd_400x400.jpg"));
            tweets.Add(new Uri("ms-appx:///Assets/ProfilePictures/d6Saonzx_400x400.jpg"));
            return tweets;
        }

        #region Constants
        const float horizontalSpace = 20.0f; //space between images
        const float verticalSpace = 80.0f; //space between images
        const float imageDimension = 100.0f; //width and height of images 
        #endregion

    }
}