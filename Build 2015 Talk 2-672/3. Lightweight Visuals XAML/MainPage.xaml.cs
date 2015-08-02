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

namespace _3.Lightweight_Visuals_XAML
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
            // Create Solid Color Background
            SolidColorVisual background = _compositor.CreateSolidColorVisual();
            background.Size = new System.Numerics.Vector2((float)this.ActualWidth, (float)this.ActualHeight);
            background.Color = Color.FromArgb(0xFF, 0x21, 0xBD, 0xEE);

            // put it at the bottom of the visual tree
            _rootVisual.Children.InsertAtBottom(background);

            // use current position for laying out as we iterate
            Vector3 currentGridPosition = new Vector3(horizontalSpace, horizontalSpace, 1.0f);
            Vector2 imageSize = new Vector2(imageDimension, imageDimension);

            foreach (Uri profileImageUri in GetProfileUrisForTwitterFollowers("@wincomposition"))
            {
                AddImageVisualForFollower(profileImageUri, currentGridPosition, imageSize);

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

        private void AddImageVisualForFollower(Uri imageUri, Vector3 offset, Vector2 size)
        {
            //Load profile image into a CompositionImage
            CompositionImage profilePic =
                _compositor.DefaultGraphicsDevice.CreateImageFromUri(imageUri);

            //Add main profile picture visual
            ImageVisual profilePicVisual = _compositor.CreateImageVisual();
            profilePicVisual.Image = profilePic;
            profilePicVisual.Stretch = CompositionStretch.UniformToFill;
            profilePicVisual.Size = size;
            profilePicVisual.Offset = offset;

            //Add ImageVisual to tree
            _rootVisual.Children.InsertAtTop(profilePicVisual);
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
