using ExampleGallery;
using Microsoft.Graphics.Canvas;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace _7.Win2D_Interop
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        CanvasSwapChain _swapchain;
        CanvasDevice _device;
        Size sizeSwapchain = new Size(250, 100);

        public MainPage()
        {
            this.InitializeComponent();
            Loaded += MainPage_Loaded;
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Create the device resources
            CreateResources(sizeSwapchain);
        }

        private void CreateResources(Size sizeSwapchain)
        {
            _device = new CanvasDevice();
            _swapchain = new CanvasSwapChain(_device, (float)sizeSwapchain.Width, (float)sizeSwapchain.Height, 96);

            // Start rendering the swapchain
            StartRendering(sizeSwapchain);
        }

        private void StartRendering(Size sizeSwapchain)
        {
            float ticks = 0;

            BurningTextExample fire = new BurningTextExample();
            fire.CreateResources();

            Task.Factory.StartNew(() =>
            {
                while (true)
                {
                    using (var ds = _swapchain.CreateDrawingSession(Colors.Transparent))
                    {
                        // HACK - this should be time in seconds since animation start
                        ticks += .016f;

                        fire.Draw(ds, sizeSwapchain, ticks);
                    }
                    _swapchain.Present();
                }

            }, TaskCreationOptions.LongRunning);
        }

        private void CreateVisualAndAttachSwapchain(UIElement parent, Size sizeSwapchin)
        {
            ContainerVisual container = (ContainerVisual)Windows.UI.Xaml.Hosting.ElementCompositionPreview.GetContainerVisual(parent);

            // Create the ImageVisual to draw the swapchain
            var imageVisual = container.Compositor.CreateImageVisual();
            imageVisual.Size = new System.Numerics.Vector2((float)sizeSwapchin.Width, (float)sizeSwapchin.Height);

            var surface = Win2DCompositionInterop.Surface.CreateCompositionSurface(container.Compositor, _swapchain);

            if (surface != null)
            {
                imageVisual.Image = surface;

                container.Children.InsertAtTop(imageVisual);
            }
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            // Create the visual, add it to the tree and attach the swapchain
            CreateVisualAndAttachSwapchain(button, sizeSwapchain);

            //// Start rendering the swapchain
            //StartRendering(sizeSwapchain);
        }
    }
}
