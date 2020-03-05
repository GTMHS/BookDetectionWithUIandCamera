#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QMessageBox"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	ui->lcdNumber->display(sum);
	readFiles();
}

QImage cvMat2QImage(const cv::Mat& mat);

void MainWindow::readFiles() {
	Mat image;
	stringstream ss;
	string imagefile = "D:\\Code\\Pic\\Pic (";
	try
	{
		string outfile;
		Mat image_for_write;
		for (int i = 1; i < 16; i++) {
			ss << imagefile << i << ").bmp";
			string infile = ss.str();
			image_for_write = imread(infile);
			QImage img = cvMat2QImage(image_for_write);
			QPixmap pixmap = QPixmap::fromImage(img);
			ui->label->setPixmap(pixmap);
			Rect rect = Rect(69, 1387, 3000, 399);
			image_for_write = image_for_write(rect);
			QString strQ = QString::fromLocal8Bit(infile.c_str());
			bookdetection(strQ, image_for_write);
			img = cvMat2QImage(image_for_write);
			pixmap = QPixmap::fromImage(img);
			ui->label_2->setPixmap(pixmap);	
			waitKey(2000);
			ss.str("");
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
		ui->label_3->setText(e.what());
		throw;
	}

}

QImage cvMat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}

bool sortFun(Rect p1, Rect p2);
bool MainWindow::bookdetection(QString filename,Mat imagefile) {
	//outfile = "F:\\ͼ��\\20191214\\20191217\\" + get_datetime() + ".bmp";
	string outfile = "E:\\pic\\label\\" + get_datetime() + ".bmp";
	String modelConfiguration = "D:/yolov3.cfg";
	/*String model_label_Weights = "D:/yolov3-voc_last1024.weights";*/
	String model_label_Weights = "D:/yolov3-voc_9000.weights";

	vector<Rect> boxes = detect_image(imagefile, model_label_Weights, modelConfiguration);
	//���򣬸��ݵõ��ķ�����е��������������򣬰���y��С�����˳����
	sort(boxes.begin(), boxes.end(), sortFun);
	vector<Point> points;
	int max_point_x = 0, max_point_y = 0;
	double average_piexl_value = 0;

	//forѭ����ȡÿ���ڿ���е㣬���洢��points��
	for (int i = 0; i < boxes.size(); i++) {
		points.push_back(Point(boxes[i].x + 0.5*boxes[i].width, boxes[i].y + 0.5*boxes[i].height));
		average_piexl_value = average_piexl_value + imagefile.at<uchar>(points[i]);
		//if (boxes[i].x > max_point_x) {
		//	max_point_x = boxes[i].x;
		//	max_point_y = boxes[i].y;
		//}
		//if (i > 0)
		//	cout << "Rect(" << boxes[i].x << ", " << boxes[i].y << ", " << boxes[i].width << ", " << boxes[i].height << ")" << "Mid-points " << ":" << points[i].x << "," << points[i].y << " �����ࣺ" << points[i - 1].x - points[i].x << ", " << points[i].y - points[i - 1].y << endl;
		//else
		//	cout << "Rect(" << boxes[i].x << ", " << boxes[i].y << ", " << boxes[i].width << ", " << boxes[i].height << ")" << "Mid-points " << ":" << points[i].x << "," << points[i].y << endl;
		//cout << "Mid-points " << i << "," << points[i].x << "," << points[i].y << endl;
		//cout << i << ": boxes[i].x " << boxes[i].x << " boxes[i].y " << boxes[i].y << " boxes[i].width " << boxes[i].width << " boxes[i].height " << boxes[i].height << endl;
	}
	//if (boxes.size() < block_nums) {
	//	int this_boxes_size = boxes.size();
	//	//������е�����λ��
	//	for (int i = 0; i <= points.size(); i++) {
	//		//�����ǰ����е�����һ������е�����λ�õ�y��5������Ϊ�˿��λ����ȷ
	//		if (abs(abs(points[i].y - points[i + 1].y) - relative_locations[i].y) <= 10) {
	//			cout << "Correct point" << i << endl;
	//			continue;
	//		}
	//		//���������Ƿ���ȱʧ��
	//		else
	//		{
	//			//;�����ȱʧ����һ����
	//			//�ҵ���һ��������Ͻ�λ��,x�ǴӴ�С��y�Ǵ�С����
	//			int x_1 = points[i].x - relative_locations[i].x - 0.5*locations[i + 1].width;
	//			int y_1 = points[i].y + relative_locations[i].y - 0.5*locations[i + 1].height;
	//			Rect r = Rect(x_1, y_1, locations[i + 1].width, locations[i + 1].height);
	//			average_piexl_value = average_piexl_value / boxes.size() + 10;
	//			Mat imagefile1 = imagefile(r);
	//			double value = 0;
	//			for (int i = 0; i < imagefile1.rows; ++i)
	//				for (int j = 0; j < imagefile1.cols; ++j)
	//					value += imagefile1.at<uchar>(i, j);
	//			value = value / (imagefile1.cols*imagefile1.rows);
	//			if (value < average_piexl_value)
	//			{
	//				this_boxes_size++;
	//				rectangle(imagefile, r, Scalar(255, 178, 50), 2);
	//				cout << "ȱʧ��λ��������ȷ" << endl;
	//				imwrite(outfile, imagefile);
	//				cout << "�ļ�д�룺" + outfile << endl << endl;
	//				if (this_boxes_size < points.size()) 
	//					continue;
	//				else
	//					return 1;
	//			}
	//			else
	//				//cout << "ȱʧ��λ����������!" << endl;
	//				continue;
	//				//return false;				
	//		}
	//	}
	//	return 1;
	//}
	//ǰʮ����������Ӧ�����ؾ�б�ʸ����ϵ������
	//if (total_number < 11) {
	//	//v_block_locations.push_back((boxes));
	//}
	//if (total_number == 10) {
	//	double sum_k = 0, sum_b = 0, sum_r_q = 0;
	//	for (int i = 0; i < 11; i++) {
	//		sum_b += v_b[i];
	//		sum_k += v_k[i];
	//		sum_r_q += v_r_q[i];
	//	}
	//	cout << "sum_b: " << sum_b << " sum_k��" << sum_k << " sum_r_q: " << sum_r_q << endl;
	//	k = sum_k / 10;
	//	b = sum_b / 10;
	//	r_q = sum_r_q / 10;
	//	cout << "k: " << k << " b: " << b << " r_q: " << r_q;
	//}
	//ǰʮ����������Ӧ�����ؾ�б�ʸ����ϵ������
	//if (boxes.size() == block_nums && total_number < 11) {
	//	LinearFitting(points, 0, 0, 0);
	//}

	//cout << "�����ɫ��������� " << boxes.size() << endl;
	int this_block_nums = boxes.size();
	sum += 1;
	ui->lcdNumber->display(sum);
	if (this_block_nums == 11) {
		sum_of_correct += 1;
		ui->lcdNumber_2->display(sum_of_correct);
	}
	else
	{
		sum_of_wrong += 1;
		ui->lcdNumber_3->display(sum_of_wrong);
	}
	//int val = 0;
	//if (this_block_nums > block_nums) {
	//	for (int i = 0; i < boxes.size(); i++) {
	//		//if (boxes[i].width > 100) {
	//		//	boxes.erase(boxes.begin() + i);
	//		//}
	//	}
	//	//if (boxes.size() == block_nums) {
	//	//	return 1;
	//	//}
	//}	
	//if (boxes.size() < block_nums) {
	//	//������е�����λ��
	//	for (int i = 0; i <= points.size(); i++) {
	//		//�����ǰ����е�����һ������е�����λ�õ�y��5������Ϊ�˿��λ����ȷ
	//		if (abs(abs(points[i].y - points[i + 1].y) - relative_locations[i].y) <= 10) {
	//			cout << "Correct point" << i << endl;
	//			continue;
	//		}
	//		//���������Ƿ���ȱʧ��
	//		else
	//		{
	//			//;�����ȱʧ����һ����
	//			////�ҵ���һ��������Ͻ�λ��,x�ǴӴ�С��y�Ǵ�С����
	//			//int x_1 = points[i].x - relative_locations[i].x - 0.5*locations[i + 1].width;
	//			//int y_1 = points[i].y + relative_locations[i].y - 0.5*locations[i + 1].height;
	//			//����y��С�����˳������
	//			int x_1 = points[i].x - relative_locations[i].x - 0.5*locations[i + 1].width + 0.5*locations[i].width;
	//			int y_1 = points[i].y + relative_locations[i].y - 0.5*locations[i + 1].height + 0.5*locations[i].height;
	//			Rect r = Rect(x_1, y_1, locations[i + 1].width, locations[i + 1].height);
	//			average_piexl_value = average_piexl_value / boxes.size() + 10;
	//			Mat imagefile1 = imagefile(r);
	//			double value = 0;
	//			//ѭ���������úڿ��е��������ؼ���ƽ�ֻҶ�ֵ
	//			for (int i = 0; i < imagefile1.rows; ++i) for (int j = 0; j < imagefile1.cols; ++j) value += imagefile1.at<uchar>(i, j);
	//			value = value / (imagefile1.cols*imagefile1.rows);
	//			if (value < average_piexl_value)
	//			{  
	//				rectangle(imagefile, r, Scalar(255, 108, 50), 2);
	//				cout << "ȱʧ��λ��������ȷ" << endl;
	//				boxes.push_back(r);
	//				continue;
	//			}
	//			else
	//			{
	//				cout << "ȱʧ��λ����������!" << endl;
	//				continue;
	//			}
	//		}
	//	}			
	//}

	QString str = filename + " num of blocks: ";
	str = str + QString::number(this_block_nums);
	ui->label_3->setText(str);
	//ui->label_3->setText(QStringLiteral(filename + " �����ɫ��־��Ϊ " + QString::number(this_block_nums)));

	//ui->label_3->append("this_block_nums is " + boxes.size());
	//switch (this_block_nums)
	//{
	//case 11:
	//	cout << "��ɫ��־��������ȷ" << endl;
	//	imwrite(outfile, imagefile);
	//	cout << "�ļ�д�룺" + outfile << endl;
	//	return LinearFitting(points, -0.34, 0, 0.98);
	//case 12:
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
	//	cout << "��ɫ��־����Ϊ12" << endl;
	//	outfile = "E:\\pic\\label\\12-" + get_datetime() + ".bmp";
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//	imwrite(outfile, imagefile);
	//	cout << "�ļ�д�룺" + outfile << endl;
	//	return LinearFitting(points, -0.34, 0, 0.98);
	//case 10:
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
	//	cout << "��ɫ��־����Ϊ10" << endl;
	//	outfile = "E:\\pic\\label\\��" + get_datetime() + ".bmp";
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//	imwrite(outfile, imagefile);
	//	cout << "�ļ�д�룺" + outfile << endl;
	//	return LinearFitting(points, -0.34, 0, 0.98) && 0;
	//default:
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	//	cout << "��ɫ��־��������  " << this_block_nums << endl;
	//	outfile = "E:\\pic\\label\\����" + get_datetime() + ".bmp";
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//	imwrite(outfile, imagefile);
	//	cout << "�ļ�д�룺" + outfile << endl;
	//	return 0;
	//}

	//if (this_block_nums == block_nums)
	//	cout << "��ɫ��־��������ȷ" << endl;
	//else if (this_block_nums == 12) {
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);
	//	cout << "��ɫ��־����Ϊ12" << endl;
	//	outfile = "D:\\20200110\\label\\9000\\12-" + get_datetime() + ".bmp";
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//}
	//else
	//{
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	//	if (boxes.size() > block_nums) {
	//		cout << "��ɫ��־�ࣺ  " << boxes.size() << "\t" << block_nums << endl;
	//		outfile = "D:\\20200110\\label\\9000\\��" + get_datetime() + ".bmp";
	//	}
	//	else
	//	{
	//		if (boxes.size() < block_nums)
	//		{
	//			cout << "��ɫ��־�٣�  " << boxes.size() << "\t" << block_nums << endl;
	//			outfile = "D:\\20200110\\label\\9000\\��" + get_datetime() + ".bmp";
	//		}
	//	}
	//	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//}
	//if (boxes.size() == block_nums) {
	//	return LinearFitting(points, 0, 0, 0); //����Ƿ�����
	//}
	//else if (boxes.size() > block_nums) {
	//	cout << "Error" << endl;
	//	return false;
	//}
	//else if (boxes.size() < block_nums)
	//{
	//	for (int i = 0; i < block_nums; i++) {
	//		if (iscontained(templateRect[i], points[i]))
	//			continue;
	//		else
	//		{
	//			average_piexl_value = average_piexl_value / boxes.size() + 10;
	//			imagefile = imagefile(templateRect[i]);
	//			double value = 0;
	//			for (int i = 0; i < imagefile.rows; ++i)
	//				for (int j = 0; j < imagefile.cols; ++j)
	//					value += imagefile.at<uchar>(i, j);
	//			value = value / (imagefile.cols*imagefile.rows);
	//			if (value < average_piexl_value)
	//			{
	//				cout << "Normal" << endl;
	//				return true;
	//			}
	//			else
	//			{
	//				cout << "Abnormal!" << endl;
	//				return false;					
	//			}
	//		}
	//	}		
	//}
	return true;
}

//OpenCV Mat��QImage�໥ת������
Mat MainWindow::QImage2cvMat(QImage &image, bool clone, bool rb_swap)
{
	cv::Mat mat;
	//qDebug() << image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void *)image.constBits(), image.bytesPerLine());
		if (clone)  mat = mat.clone();
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
		if (clone)  mat = mat.clone();
		if (rb_swap) cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
	case QImage::Format_Grayscale8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void *)image.bits(), image.bytesPerLine());
		if (clone)  mat = mat.clone();
		break;
	}
	return mat;
}

bool MainWindow::LinearFitting(const vector<Point> points, double slope, double intercept, double r_square)
{
	int length = points.size();
	double xmean = 0.0;
	double ymean = 0.0;
	for (int i = 0; i < length; i++)
	{
		xmean += points[i].x;
		ymean += points[i].y;
	}
	xmean /= length;
	ymean /= length;

	double sumx2 = 0.0;
	double sumy2 = 0.0;
	double sumxy = 0.0;
	for (int i = 0; i < length; i++)
	{
		sumx2 += (points[i].x - xmean) * (points[i].x - xmean);
		sumy2 += (points[i].y - ymean) * (points[i].y - ymean);
		sumxy += (points[i].y - ymean) * (points[i].x - xmean);
	}
	double slope1 = sumxy / sumx2;  //б��
	double intercept1 = ymean - slope * xmean; //�ؾ�
	double r_square1 = sumxy * sumxy / (sumx2 * sumy2); //���ϵ��
	cout << "y = " << slope1 << "x+ " << intercept1 << "  r_square1 is " << r_square1 << endl;

	if (abs(r_square - r_square1) <= 0.1 && abs(slope - slope1) <= 0.1) {
		cout << "���ϵ����ȷ" << endl;

		QString str = "y=" + QString::number(slope) + "x+ " + QString::number(intercept1) + "  r_square1 is " + QString::number(r_square1) + "Correct";
		ui->label_3->setText(str);
		return true;
	}
	else
	{

		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | BACKGROUND_RED);
		cout << "���ϵ������" << endl;
		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		QString str = "y=" + QString::number(slope) + "x+ " + QString::number(intercept1) + "  r_square1 is " + QString::number(r_square1) + "Wrong";
		ui->label_3->setText(str);
		return false;
	}

}

string MainWindow::get_datetime()
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	char temp[100];
	sprintf_s(temp, "%d-%02d-%02d-%02d-%02d-%02d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond
	);
	std::string params = string(temp);
	return params;
}

vector<Rect> MainWindow::detect_image(Mat frame, string modelWeights, string modelConfiguration) {
	// Load the network
	Net net = readNetFromDarknet(modelConfiguration, modelWeights);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	//net.setPreferableTarget(DNN_TARGET_OPENCL);
	net.setPreferableTarget(DNN_TARGET_CPU);

	Mat blob;
	blobFromImage(frame, blob, 1 / 255.0, cvSize(inpWidth, inpHeight), Scalar(0, 0, 0), true, false);

	//Sets the input to the network
	net.setInput(blob);

	// Runs the forward pass to get output of the output layers
	vector<Mat> outs;
	net.forward(outs, getOutputsNames(net));

	// Remove the bounding boxes with low confidence
	vector<Rect> boxes = postprocess_return(frame, outs);
	// Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
	vector<double> layersTimes;
	double freq = getTickFrequency() / 1000;
	double t = net.getPerfProfile(layersTimes) / freq;
	string label = format("Inference time for a frame : %.2f ms", t);
	//cout << label<<endl;
	//putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
	// Write the frame with the detection boxes
	//imshow(kWinName, frame);
	//imwrite(classesFile, frame);
	return boxes;
	//cv::waitKey(30);
}

bool sortFun(Rect p1, Rect p2) {
	return p1.y + 0.5*p1.height < p2.y + 0.5* p2.height;
}

vector<Rect> MainWindow::postprocess_return(Mat& frame, const vector<Mat>& outs)
{
	vector<int> classIds;
	vector<float> confidences;
	vector<Rect> boxes;
#pragma omp parallel for
	for (int i = 0; i < outs.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > confThreshold)
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(Rect(left, top, width, height));
			}
		}
	}

	// Perform non maximum suppression to eliminate redundant overlapping boxes with
	// lower confidences
	vector<int> indices;
	NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
	//cout << "indices.size(ƥ��õ���Ŀ����):" << indices.size() << endl;
	vector<Rect> boxes_for_return;

	Rect box = boxes[0];

	for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		Rect box = boxes[idx];
		if (box.x < 0)
		{
			box.width = box.width + box.x;
			box.x = 0;
		}
		if (box.width > frame.cols) {
			box.width = frame.cols - box.x;
		}
		if (box.x + box.width > frame.cols) {
			box.width = frame.cols - box.x;
		}
		boxes_for_return.push_back(box);
		//drawPred(classIds[idx], confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame);
		//����
		rectangle(frame, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(255, 178, 50), 2);
	}

	return boxes_for_return;
}

vector<String> MainWindow::getOutputsNames(const Net& net)
{
	static vector<String> names;
	if (names.empty())
	{
		//Get the indices of the output layers, i.e. the layers with unconnected outputs
		vector<int> outLayers = net.getUnconnectedOutLayers();

		//get the names of all the layers in the network
		vector<String> layersNames = net.getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}

//QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
//{
//	// 8-bits unsigned, NO. OF CHANNELS = 1
//	if (mat.type() == CV_8UC1)
//	{
//		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
//		// Set the color table (used to translate colour indexes to qRgb values)
//		image.setColorCount(256);
//		for (int i = 0; i < 256; i++)
//		{
//			image.setColor(i, qRgb(i, i, i));
//		}
//		// Copy input Mat
//		uchar *pSrc = mat.data;
//		for (int row = 0; row < mat.rows; row++)
//		{
//			uchar *pDest = image.scanLine(row);
//			memcpy(pDest, pSrc, mat.cols);
//			pSrc += mat.step;
//		}
//		return image;
//	}
//	// 8-bits unsigned, NO. OF CHANNELS = 3
//	else if (mat.type() == CV_8UC3)
//	{
//		// Copy input Mat
//		const uchar *pSrc = (const uchar*)mat.data;
//		// Create QImage with same dimensions as input Mat
//		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
//		return image.rgbSwapped();
//	}
//	else if (mat.type() == CV_8UC4)
//	{
//		// Copy input Mat
//		const uchar *pSrc = (const uchar*)mat.data;
//		// Create QImage with same dimensions as input Mat
//		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
//		return image.copy();
//	}
//	else
//	{
//		return QImage();
//	}
//}