#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/core/core_c.h"
//#include "opencv2/highgui/highgui_c.h"
//#include "opencv2/imgproc/imgproc_c.h"
//#include "opencv2/video/video.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
//#include "opencv2/videostab/videostab.hpp"
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/flann/miniflann.hpp"
//#include "opencv2/photo/photo.hpp"
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/ml/ml.hpp"
//#include "opencv2/contrib/contrib.hpp"
//#include "opencv2/ts/ts.hpp"
//#include "opencv2/stitching/stitcher.hpp"
//#include "opencv2/legacy/legacy.hpp"

#ifdef _DEBUG
        #pragma comment(lib,"opencv_core249d.lib")
        #pragma comment(lib,"opencv_highgui249d.lib")
        #pragma comment(lib,"opencv_imgproc249d.lib")
//      #pragma comment(lib,"opencv_video249d.lib")
//      #pragma comment(lib,"opencv_nonfree249d.lib")
//      #pragma comment(lib,"opencv_videostab249d.lib")
//      #pragma comment(lib,"opencv_features2d249d.lib")
//      #pragma comment(lib,"opencv_objdetect249d.lib")
//      #pragma comment(lib,"opencv_flann249d.lib")
//      #pragma comment(lib,"opencv_photo249d.lib")
//      #pragma comment(lib,"opencv_calib3d249d.lib")
//      #pragma comment(lib,"opencv_ml249d.lib")
//      #pragma comment(lib,"opencv_contrib249d.lib")
//      #pragma comment(lib,"opencv_ts249d.lib")
//      #pragma comment(lib,"opencv_stitching249d.lib")
//      #pragma comment(lib,"opencv_legacy249d.lib")
#else
        #pragma comment(lib,"opencv_core249.lib")
        #pragma comment(lib,"opencv_highgui249.lib")
        #pragma comment(lib,"opencv_imgproc249.lib")
//      #pragma comment(lib,"opencv_video249.lib")
//      #pragma comment(lib,"opencv_nonfree249.lib")
//      #pragma comment(lib,"opencv_videostab249.lib")
//      #pragma comment(lib,"opencv_features2d249.lib")
//      #pragma comment(lib,"opencv_objdetect249.lib")
//      #pragma comment(lib,"opencv_flann249.lib")
//      #pragma comment(lib,"opencv_photo249.lib")
//      #pragma comment(lib,"opencv_calib3d249.lib")
//      #pragma comment(lib,"opencv_ml249.lib")
//      #pragma comment(lib,"opencv_contrib249.lib")
//      #pragma comment(lib,"opencv_ts249.lib")
//      #pragma comment(lib,"opencv_stitching249.lib")
//      #pragma comment(lib,"opencv_legacy249.lib")
#endif