/*
* Copyright (c) 2017-2020 CelePixel Technology Co. Ltd. All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#define MAT_ROWS 800
#define MAT_COLS 1280

using namespace std;
using namespace cv;
using namespace Eigen;

//Writes event data from file to vector
vector<vector<int> > writeFile2Vec (string filename){
    ifstream infile_feat(filename.c_str());
    string feature;
    float feat_onePoint;
    vector<int> lines;
    vector<vector<int> > lines_feat;
    lines_feat.clear();

    if (infile_feat.is_open()){
        while(!infile_feat.eof())
        {
            getline(infile_feat, feature);
            stringstream stringin(feature);
            lines.clear();
            while (stringin >> feat_onePoint) {
                lines.push_back(feat_onePoint);
            }
            lines_feat.push_back(lines);
        }
        infile_feat.close();
        return lines_feat;
    }
    else{
        cout << "Could not access " << filename << "\nFile may not exist..." << endl;
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    //Print ASCII art
    cout << "=======================================================" << endl;
    cout << " _____  _           __     ______          _____ _  __ " << endl;
    cout << "|  __ \\| |        /\\\\ \\   / /  _ \\   /\\   / ____| |/ / " << endl;
    cout << "| |__) | |       /  \\\\ \\_/ /| |_) | /  \\ | |    | ' / " << endl;
    cout << "|  ___/| |      / /\\ \\\\   / |  _ < / /\\ \\| |    |  <  " << endl;
    cout << "| |    | |____ / ____ \\| |  | |_) / ____ \\ |____| . \\ " << endl;
    cout << "|_|    |______/_/    \\_\\_|  |____/_/    \\_\\_____|_|\\_\\" << endl;
    cout << "=======================================================" << endl;

    //Load event data to vector
    string filename;

    if (argc == 2){
        filename.append("../../Recordings/Downsampled/");
        filename.append(string(argv[1]));
    }
    else{
        cout << "Please enter the name of the file!" << endl;
        return 0;
    }

    cout << "Reading events from file..." << endl;

    vector<vector<int> > vecEvent = writeFile2Vec(filename);

    //Displaying of events
    cout << "Displaying events..." << endl;
    int counter = 0;
    Mat evMat = cv::Mat::zeros(cv::Size(MAT_COLS/2, MAT_ROWS/2), CV_8UC1);
    int datasize = vecEvent.size() - 1;

    //Cycle through event vector
    for (int i = 0; i < datasize; i++){
        int y = vecEvent[i][1];
        int x = vecEvent[i][2];

        evMat.at<uchar>(x, y) = 255;
        counter++;

        if (counter == datasize/2000){
            imshow("Event Pic", evMat);
            waitKey(1);
            evMat = 0;
            counter = 0;
        }
    }

    cout << "Finished!" << endl;
    return 1;
}
