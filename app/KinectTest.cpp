#include <iostream>
#include <pcl/io/openni2_grabber.h>

#include "io/device.h"
#include "cloud/cloud_pipe.h"

using namespace telef::io;
using namespace telef::types;
using namespace telef::cloud;

/**
 * Continuously get frames of point cloud and image.
 *
 * Prints size of pointcloud and size of the image on every frame received
 * Remove all points that have NaN Position on Receive.
 * You can check this by watching varying number of pointcloud size
 */

int main(int ac, char* av[]) 
{
    pcl::io::OpenNI2Grabber::Mode depth_mode = pcl::io::OpenNI2Grabber::OpenNI_Default_Mode;
    pcl::io::OpenNI2Grabber::Mode image_mode = pcl::io::OpenNI2Grabber::OpenNI_Default_Mode;

    auto grabber = std::make_unique<TelefOpenNI2Grabber>("#1", depth_mode, image_mode);

    auto imagePipe = std::make_shared<IdentityPipe<ImageT>>();
    auto cloudPipe = std::make_shared<IdentityPipe<CloudConstT>>();
    auto cloudPipe2 = std::make_shared<RemoveNaNPoints>();
    auto cloudPipe3 = std::make_shared<IdentityPipe<CloudConstT>>();
    auto cloudCombinedPipe = cloudPipe
            ->then<CloudConstT>(cloudPipe2)
            ->then<CloudConstT>(cloudPipe3);

    auto imageChannel = std::make_shared<DummyImageChannel<ImageT>>(std::move(imagePipe));
    auto cloudChannel = std::make_shared<DummyCloudChannel<CloudConstT>>(std::move(cloudCombinedPipe));

    auto merger = std::make_shared<DummyImageCloudMerger>();
    auto frontend = std::make_shared<DummyCloudFrontEnd>();

    ImagePointCloudDevice<CloudConstT, ImageT, CloudConstT, CloudConstT> device {std::move(grabber)};
    device.addCloudChannel(cloudChannel);
    device.addImageChannel(imageChannel);
    device.addImageCloudMerger(merger);
    device.addFrontEnd(frontend);

    device.run();

    return 0;
}

