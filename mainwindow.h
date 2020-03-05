#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\dnn.hpp>
#include <string>
#include <iostream>
#include <string>
#include <atlstr.h>


using namespace cv;
using namespace std;
using namespace dnn;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	float confThreshold = 0.5; // Confidence threshold
	float nmsThreshold = 0.4;  // Non-maximum suppression threshold
	int inpWidth = 416;  // Width of network's input image
	int inpHeight = 416; // Height of network's input image

	bool bookdetection(QString filename, Mat imagefile);
	void readFiles();
	int sum = 0, sum_of_wrong = 0, sum_of_correct = 0;
	
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

	bool LinearFitting(const vector<Point> points, double slope, double intercept, double r_square);

	string get_datetime();

	vector<Rect> detect_image(Mat frame, string modelWeights, string modelConfiguration);
	vector<String> getOutputsNames(const Net& net);
	vector<Rect> postprocess_return(Mat& frame, const vector<Mat>& out);
	Mat QImage2cvMat(QImage &image, bool clone, bool rb_swap);
};

#endif // MAINWINDOW_H
