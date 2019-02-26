/*
 * Separator Marker Detector
 * Copyright (C) 2018-2019 - Alexis Maldonado, Universitaet Bremen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * */


#include <ros/ros.h>
#include <iostream>
#include <sensor_msgs/image_encodings.h>
#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/Marker.h>

#include <refills_msgs/Separator.h>
#include <refills_msgs/SeparatorArray.h>

#include <asr_halcon_bridge/halcon_image.h>

// Using image_transport for publishing and subscribing to images in ROS
#include <image_transport/image_transport.h>

//Halcon includes
#include "HalconCpp.h"
#include "HDevThread.h"
//#include <X11/Xlib.h>


using namespace HalconCpp;

class SeparatorDetector{
  //ROS class members
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Publisher data_pub_;
  ros::Publisher marker_pub_;

  // Working class members (manually created)
  HImage *halcon_msg_image;
  sensor_msgs::Image image_msg;
  halcon_bridge::HalconImage hbridge_img;
  HImage display_image;




  //Halcon class members follow (from the hdevelop c++ export)
  // Local iconic variables
  HObject  ho_Image, ho_ModelRegion, ho_TemplateImage;
  HObject  ho_ModelContours, ho_TransContours, ho_ObjectSelected;
  HObject  ho_ObjectSelectedWorld, ho_RectifiedImage;

  // Local control variables
  HTuple  hv_CameraParameters, hv_CameraPose, hv_CamParam;
  HTuple  hv_CamPose, hv_ModelID, hv_ImageWidth, hv_ImageHeight;
  HTuple  hv__X, hv__Y, hv_ImageScale, hv_ModelRegionArea;
  HTuple  hv_RefRow, hv_RefColumn, hv_DX, hv_DY, hv_ModelPose;
  HTuple  hv_CamParRect, hv_NumModelContours, hv_TmpHomMat3D;
  HTuple  hv_I, hv__Z, hv__Xc, hv__Yc, hv__Zc, hv__R, hv__C;
  HTuple  hv_TestImages, hv_T, hv_ResultPose, hv_ResultCovariance;
  HTuple  hv_Score, hv_RectificationPose, hv_RectificationHomMat3D;
  HTuple  hv_I2;



public:
  SeparatorDetector()
    : nh_("~"), it_(nh_){
    // Subscribe and  publish using image_transport
    image_sub_ = it_.subscribe("image_in", 3, &SeparatorDetector::imageCb, this); // Main source of images
    image_pub_ = it_.advertise("image_out", 3);
    data_pub_ = nh_.advertise<refills_msgs::SeparatorArray>("data_out", 10);
    marker_pub_ = nh_.advertise<visualization_msgs::MarkerArray>("markers_out", 10);
  }

  ~SeparatorDetector()
  {
    //Matching 01: Clear model when done
    ClearDeformableModel(hv_ModelID);
  }

  bool init() {
    //Model initialization for the matching algorithm
    // Calibration 01: Code generated by Calibration 01
    //ReadCamPar("/home/amaldo/dm_marker_samples/cam_calib/rgb_wrist.cal", &hv_CameraParameters);
    //ReadPose("/home/amaldo/dm_marker_samples/cam_calib/rgb_wrist_pose_to_ref_marker.dat",
    //  &hv_CameraPose);


    //
    //Matching 01: ************************************************
    //Matching 01: BEGIN of generated code for model initialization
    //Matching 01: ************************************************
    //
    //Matching 01: Initialize calibration
    hv_CamParam.Clear();
    hv_CamParam[0] = "area_scan_division";
    hv_CamParam[1] = 0.0164374;
    hv_CamParam[2] = -102.241;
    hv_CamParam[3] = 3.45033e-06;
    hv_CamParam[4] = 3.45e-06;
    hv_CamParam[5] = 2082.16;
    hv_CamParam[6] = 1392.79;
    hv_CamParam[7] = 4096;
    hv_CamParam[8] = 3000;
    hv_CamPose.Clear();
    hv_CamPose[0] = 0.0188459;
    hv_CamPose[1] = -0.0116134;
    hv_CamPose[2] = 0.631195;
    hv_CamPose[3] = 356.449;
    hv_CamPose[4] = 359.438;
    hv_CamPose[5] = 269.594;
    hv_CamPose[6] = 0;
   
    //for ARRINA:
    //CameraParameters := ['area_scan_division',0.0161555,-198.596,3.44973e-06,3.45e-06,2107.25,1498.04,4096,3000]
    hv_CamParam.Clear();
    hv_CamParam[0] = "area_scan_division";
    hv_CamParam[1] = 0.0161555;
    hv_CamParam[2] = -198.596;
    hv_CamParam[3] = 3.44973e-06;
    hv_CamParam[4] = 3.45e-06;
    hv_CamParam[5] = 2107.25;
    hv_CamParam[6] = 1498.04;
    hv_CamParam[7] = 4096;
    hv_CamParam[8] = 3000;
    hv_CamPose.Clear();
    //CamPose := [0.077491,-0.00216746,0.551575,359.077,1.17243,270.846,0]
    hv_CamPose[0] = 0.077491;
    hv_CamPose[1] = -0.00216746;
    hv_CamPose[2] = 0.551575;
    hv_CamPose[3] = 359.077;
    hv_CamPose[4] = 1.17243;
    hv_CamPose[5] = 270.846;
    hv_CamPose[6] = 0;

    //
    //Matching 01: Obtain the model image
    //For the 2MP camera
    //ReadImage(&ho_Image, "/home/amaldo/dm_marker_samples/cam_calib/template_dm_logo.hobj");
    //New file for the 12MP camera
    ReadImage(&ho_Image, "/home/amaldo/dm_marker_samples/12mp/template_dm_logo.hobj");
    //
    //Matching 01: Build the ROI from basic regions
    GenRectangle1(&ho_ModelRegion, 1445.2, 2705.64, 1520.33, 2825.26);
    //
    //Matching 01: Reduce the model template
    ReduceDomain(ho_Image, ho_ModelRegion, &ho_TemplateImage);
    //
    //Matching 01: Create the deformable model
    //CreatePlanarCalibDeformableModel(ho_TemplateImage, hv_CamParam, hv_CamPose, 3,
    //    HTuple(-20).TupleRad(), HTuple(40).TupleRad(), HTuple(1).TupleRad(), 1, 1,
    //    0.02, 1, 1, 0.02, "none", "use_polarity", (HTuple(25).Append(37)), 5, HTuple(),
    //    HTuple(), &hv_ModelID);
    CreatePlanarCalibDeformableModel(ho_TemplateImage, hv_CamParam, hv_CamPose, 4,
        HTuple(-35).TupleRad(), HTuple(70).TupleRad(), HTuple(2).TupleRad(), 0.75, 1,
        0.1, 0.75, 1, 0.1, "point_reduction_low", "use_polarity", (HTuple(65).Append(133)),
        25, HTuple(), HTuple(), &hv_ModelID);

    //Matching 01: Calculate scaling factor for back projection
    GetImageSize(ho_Image, &hv_ImageWidth, &hv_ImageHeight);
    ImagePointsToWorldPlane(hv_CamParam, hv_CamPose, HTuple(0).TupleConcat(hv_ImageHeight-1),
        (HTuple(0).Append(0)), 1.0, &hv__X, &hv__Y);
    hv_ImageScale = (HTuple(hv__Y[1])-HTuple(hv__Y[0]))/hv_ImageHeight;
    AreaCenter(ho_ModelRegion, &hv_ModelRegionArea, &hv_RefRow, &hv_RefColumn);
    //Matching 01: Calculate shift of origin
    hv_DX = (-hv_RefColumn)*hv_ImageScale;
    hv_DY = (-hv_RefRow)*hv_ImageScale;
    //
    //Matching 01: Get the model contour for transforming it later into the image
    GetDeformableModelContours(&ho_ModelContours, hv_ModelID, 1);
    //Matching 01: Transform model contour into metric space
    GetDeformableModelParams(hv_ModelID, "model_pose", &hv_ModelPose);
    GetDeformableModelParams(hv_ModelID, "model_row", &hv_RefRow);
    GetDeformableModelParams(hv_ModelID, "model_col", &hv_RefColumn);
    GetDeformableModelParams(hv_ModelID, "cam_param_rect", &hv_CamParRect);
    AffineTransContourXld(ho_ModelContours, &ho_ModelContours, (((HTuple(1).Append(0)).TupleConcat(hv_RefRow)).TupleConcat((HTuple(0).Append(1)))).TupleConcat(hv_RefColumn));
    ContourToWorldPlaneXld(ho_ModelContours, &ho_ModelContours, hv_CamParRect, hv_ModelPose,
        "m");
    CountObj(ho_ModelContours, &hv_NumModelContours);
    PoseToHomMat3d(hv_ModelPose, &hv_TmpHomMat3D);
    GenEmptyObj(&ho_TransContours);
    {
      HTuple end_val56 = hv_NumModelContours;
      HTuple step_val56 = 1;
      for (hv_I=1; hv_I.Continue(end_val56, step_val56); hv_I += step_val56)
      {
        SelectObj(ho_ModelContours, &ho_ObjectSelected, hv_I);
        GetContourXld(ho_ObjectSelected, &hv__Y, &hv__X);
        hv__Z = HTuple(hv__X.TupleLength(),0.0);
        AffineTransPoint3d(hv_TmpHomMat3D, hv__X, hv__Y, hv__Z, &hv__Xc, &hv__Yc, &hv__Zc);
        Project3dPoint(hv__Xc, hv__Yc, hv__Zc, hv_CamParam, &hv__R, &hv__C);
        GenContourPolygonXld(&ho_ObjectSelectedWorld, hv__R, hv__C);
        ConcatObj(ho_TransContours, ho_ObjectSelectedWorld, &ho_TransContours);
     }
    }

    //
    //Matching 01: Display the model contours
    //    if (HDevWindowStack::IsOpen())
    //      SetColor(HDevWindowStack::GetActive(),"green");
    //    if (HDevWindowStack::IsOpen())
    //      SetDraw(HDevWindowStack::GetActive(),"margin");
    //    if (HDevWindowStack::IsOpen())
    //      DispObj(ho_Image, HDevWindowStack::GetActive());
    //    if (HDevWindowStack::IsOpen())
    //      DispObj(ho_ModelRegion, HDevWindowStack::GetActive());
    //    if (HDevWindowStack::IsOpen())
    //      DispObj(ho_TransContours, HDevWindowStack::GetActive());
    //Matching 01: END of generated code for model initialization


    return true;

  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg) {
    halcon_bridge::HalconImagePtr halcon_ptr;

    try {
      halcon_ptr = halcon_bridge::toHalconCopy(msg);
    }
    catch (halcon_bridge::Exception &e) {
      ROS_ERROR("halcon_bridge exception: %s", e.what());
      return;
    }
    //ROS_INFO_STREAM("Encoding of image: " << halcon_ptr->encoding << " \n");

    // Process halcon_ptr->image using Halcon
    halcon_msg_image = halcon_ptr->image;


    ho_Image = *halcon_msg_image;

    // ROS_INFO("Received an image.");

    //Matching 01: Find the model
    try {
        FindPlanarCalibDeformableModel(ho_Image, hv_ModelID, HTuple(-35).TupleRad(),
            HTuple(70).TupleRad(), 0.75, 1, 0.75, 1, 0.59, 9, 0, 4, 0.8, "subpixel", "least_squares",
            &hv_ResultPose, &hv_ResultCovariance, &hv_Score);
    } catch (HException &e) {
        ROS_ERROR_STREAM("Halcon Exception: " << e.ErrorMessage() << "\n");
        return;
    }


    //After running FindPlanarCalibDeformableModel, hv_ResultPose is a HTuple of Length (num_detections * 7).
    //7 is the length for the Halcon Pose, with 6 doubles and one int describing the type of pose.
    int num_matches = hv_Score.Length();


    if (num_matches > 0){
      std::string printout;
      ROS_DEBUG_STREAM(("num_matches: " + std::to_string(num_matches)).c_str());
    }

    refills_msgs::SeparatorArray seps;
    seps.separators.clear();

    visualization_msgs::MarkerArray markers;
    markers.markers.clear();


    //Only interested in extracting position for now,
    // so ignoring the transformation to a pose with a meaningful orientation
    for (unsigned int i=0; i<num_matches; ++i) {
      //hv_I = (int)i;
      //hv_RectificationPose = hv_ResultPose.TupleSelectRange(hv_I*7,(hv_I*7)+6);
      //std::cout << hv_RectificationPose[0].D() << std::endl;
      ROS_DEBUG_STREAM("match #:"  << i <<  " x: " << hv_ResultPose[0+i*7].D() << " y: " << hv_ResultPose[1+i*7].D() << " z: " << hv_ResultPose[2+i*7].D());

      //Prepare the ros message for each
      refills_msgs::Separator sep;
      sep.detection_score = hv_Score[i].D();

      //Copy the stamp and frame from the image data
      sep.separator_pose.header.stamp = msg->header.stamp;
      sep.separator_pose.header.frame_id = msg->header.frame_id;

      sep.separator_pose.pose.position.x = hv_ResultPose[0 + i*7];
      sep.separator_pose.pose.position.y = hv_ResultPose[1 + i*7];
      sep.separator_pose.pose.position.z = hv_ResultPose[2 + i*7];

      sep.separator_pose.pose.orientation.w = 1.0;

      seps.separators.push_back(sep);

      visualization_msgs::Marker marker;
      marker.id = i;
      marker.action = visualization_msgs::Marker::ADD;
      marker.type = visualization_msgs::Marker::CUBE;
      marker.pose = sep.separator_pose.pose;
      marker.header = sep.separator_pose.header;
      marker.color.r = 0.0f;
      marker.color.g = 1.0f;
      marker.color.b = 0.0f;
      marker.color.a = 1.0f;

      marker.lifetime = ros::Duration(1.0);

      marker.scale.x = 0.01;
      marker.scale.y = 0.01;
      marker.scale.z = 0.005;

      markers.markers.push_back(marker);

    }

    data_pub_.publish(seps);
    marker_pub_.publish(markers);

  }

};

int main(int argc, char **argv) {
  ROS_INFO("Node for detecting markers on the DM separators. Starting up.");
  ros::init(argc, argv, "separator_marker_detector_node");
  SeparatorDetector sd;
  bool init_worked = sd.init();
  if (init_worked) {
    ROS_INFO("Init worked. Going to spin and react to callbacks.");
    ros::spin();
    ROS_INFO("Stopping nicely.");
    return EXIT_SUCCESS;

  } else {
    ROS_ERROR("Could not initialize halcon matching model. Exiting.");
    return EXIT_FAILURE;
  }

}
