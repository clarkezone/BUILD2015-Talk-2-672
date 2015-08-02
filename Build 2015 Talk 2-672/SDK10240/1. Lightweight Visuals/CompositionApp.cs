using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Composition;
using Windows.UI.Core;

namespace _1.Lightweight_Visuals
{
    class CompositionApp : IFrameworkView
    {
        ICoreWindow _window;

        Compositor _compositor;
        CompositionTarget _target;
        ContainerVisual _rootVisual;

        #region IFrameworkView Implementation
        public void SetWindow(CoreWindow window)
        {
            _window = window;

            InitializeCompositor();

            BuildVisualTree();
        }

        public void Load(string entryPoint)
        {
        }
        public void Run()
        {
            _window.Activate();
            _window.Dispatcher.ProcessEvents(CoreProcessEventsOption.ProcessUntilQuit);
        }

        public void Initialize(CoreApplicationView applicationView)
        {
        }

        public void Uninitialize()
        {
            _window = null;
        }
        #endregion

        #region Configure Composition Objects
        private void InitializeCompositor()
        {
            //Create Compositor, Target, RootVisual, Configure Target
            _compositor = new Compositor();

            _target = _compositor.CreateTargetForCurrentView();

            _rootVisual = _compositor.CreateContainerVisual();

            _target.Root = _rootVisual;
        }

        private void BuildVisualTree()
        {
            // Create Solid Color Background
            SolidColorVisual background = _compositor.CreateSolidColorVisual();
            background.Size = new System.Numerics.Vector2((float)_window.Bounds.Width, (float)_window.Bounds.Height);
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

                if (currentGridPosition.X + imageDimension > _window.Bounds.Width)
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
            CompositionImage profileImage =
                _compositor.DefaultGraphicsDevice.CreateImageFromUri(imageUri);

            ImageVisual profileImageVisual = _compositor.CreateImageVisual();
            profileImageVisual.Image = profileImage;
            profileImageVisual.Size = size;
            profileImageVisual.Offset = offset;

            _rootVisual.Children.InsertAtTop(profileImageVisual);

        }
        #endregion

        #region Twitter
        // At time of publishing TweetInvi does not support UWP. here is the code never the less
        // 
        //
        //private IEnumerable<Uri> LoadProfileUrisForTwitterFollowers(string twitterHandle)
        //{
        //    #region Credentials
        //    var consumerToken = yourToken;
        //    var consumerSecret = yourSecret;
        //    var accessToken = "2764887698-UZCfXtynUzOm7m1VSErhYKwY9sdP0OvddQ9RygZ";
        //    var accessTokenSecret = "8tYS6g0lRX2UeR58vXgnSHias1eMjfDYYs3m7UkJ5kTUU";
        //    TwitterCredentials.SetCredentials(accessToken, accessTokenSecret, consumerToken, consumerSecret);
        //    #endregion

        //    var user = Tweetinvi.User.GetUserFromScreenName(twitterHandle);
        //    var followers = user.GetFollowers().ToList().Select(sel => { return GetUriProfilePictureUri(sel); });

        //    return followers;
        //}

        //private Uri GetUriProfilePictureUri(IUser item)
        //{
        //    return new Uri(item.ProfileImageUrl.Replace("normal", "400x400"));
        //}

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

        #endregion

        #region Constants
        const float horizontalSpace = 20.0f; //space between images
        const float verticalSpace = 80.0f; //space between images
        const float imageDimension = 100.0f; //width and height of images 
        #endregion

        class MyViewFactory : IFrameworkViewSource
        {
            public IFrameworkView CreateView()
            {
                return new CompositionApp();
            }

            static int Main(string[] args)
            {
                CoreApplication.Run(new MyViewFactory());
                return 0;
            }
        }
    }
}
