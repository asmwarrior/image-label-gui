# image-label-gui

This is a GUI tool to quickly add annotations for an image. It try to support the Latex package: [tikz-imagelabels package](https://github.com/tcpluess/tikz-imagelabels) or [tikz-imagelabels – Put labels on images using TikZ](https://ctan.org/pkg/tikz-imagelabels?lang=en)

![main frame of the software](image/software-main-frame.png)

* An image can be loaded by click the "Load image" button, or you can just drag and drop the image to the main frame.

* When the "Draw arrow" check box is enabled, you can use the mouse to draw arrows in the image window.

* When the "Generate latex code" button get clicked, the Latex code will shown in the bottom "log" window.

* When "Draw arrow" mode is enabled, you can use the left mouse drag to draw a new arrow object in the image window. When you use the right mouse click and drag, you can drag the existing arrow object. When double clicked on the arrow's label, you can modify the text label of the arrow.

* In the bottom of the main frame, is the generated Latex code, you can copy and paste the code to your Latex document.