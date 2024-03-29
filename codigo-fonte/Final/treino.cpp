#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>
#include <armadillo>
#include "lib/sigpack/sigpack.h"
#include <time.h>
using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace arma;
using namespace sp;

#define WINDOW 800


double energia(vec b,vec a,colvec x){
    IIR_filt<double, double, double> iir_filt;
    iir_filt.set_coeffs(b, a);
    colvec Y1 = iir_filt.filter(x);
    double y1 = norm(Y1,2);
    y1 = y1*y1;
    return y1;
}


colvec extraction(colvec x){
    colvec parameters(8,fill::zeros);

    x=x-mean(x);
    //Valor medio absoluto
    parameters(0)=mean(abs(x));
    //Valor RMS
    parameters(1)=sqrt(sum(square(x))/WINDOW);
    //Waveform length
    parameters(2)=sum(abs(diff(x)));
    //Variancia
    parameters(3)=var(x);
    //Average Amplitude Change
    double Y = 0;
    for (int i = 0; i < WINDOW - 2; i++)
    {
        Y = Y + abs(x(i+1)-x(i));
    }
    parameters(4)=Y;
    //Energia em bandas
    double y1;
    double tot = norm(x,2);
    tot = tot*tot;

    

    vec b = {2.44897128e-09, 3.67345692e-08, 2.57141984e-07, 1.11428193e-06,
        3.34284579e-06, 7.35426075e-06, 1.22571012e-05, 1.57591302e-05,
        1.57591302e-05, 1.22571012e-05, 7.35426075e-06, 3.34284579e-06,
        1.11428193e-06, 2.57141984e-07, 3.67345692e-08, 2.44897128e-09};

    vec a = { 1.00000000e+00, -8.94097666e+00,  3.81274598e+01, -1.02595239e+02,
         1.94421048e+02, -2.74396921e+02,  2.97560724e+02, -2.52180163e+02,
         1.68238453e+02, -8.82785479e+01,  3.61095963e+01, -1.12999558e+01,
         2.61723273e+00, -4.23343031e-01,  4.27414269e-02, -2.02960157e-03};
    y1=energia(b,a,x);
    parameters(5)=y1/tot*100;

    vec b1 = { 1.31247236e-05,  0.00000000e+00, -1.18122513e-04,  0.00000000e+00,
         4.72490051e-04,  0.00000000e+00, -1.10247679e-03,  0.00000000e+00,
         1.65371518e-03,  0.00000000e+00, -1.65371518e-03,  0.00000000e+00,
         1.10247679e-03,  0.00000000e+00, -4.72490051e-04,  0.00000000e+00,
         1.18122513e-04,  0.00000000e+00, -1.31247236e-05};
    
    vec a1 = { 1.00000000e+00, -9.68593127e+00,  4.69901672e+01, -1.50739975e+02,
         3.56909439e+02, -6.60502621e+02,  9.88073702e+02, -1.21982331e+03,
         1.25851221e+03, -1.09246913e+03,  7.99555480e+02, -4.92238937e+02,
         2.53142229e+02, -1.07374086e+02,  3.67952917e+01, -9.85240606e+00,
         1.94788749e+00, -2.55100758e-01,  1.68137718e-02};
    
    y1=energia(b1,a1,x);
    parameters(6)=y1/tot*100;

    vec b2 = { 9.17785774e-09,  0.00000000e+00, -1.28490008e-07,  0.00000000e+00,
         8.35185054e-07,  0.00000000e+00, -3.34074022e-06,  0.00000000e+00,
         9.18703559e-06,  0.00000000e+00, -1.83740712e-05,  0.00000000e+00,
         2.75611068e-05,  0.00000000e+00, -3.14984077e-05,  0.00000000e+00,
         2.75611068e-05,  0.00000000e+00, -1.83740712e-05,  0.00000000e+00,
         9.18703559e-06,  0.00000000e+00, -3.34074022e-06,  0.00000000e+00,
         8.35185054e-07,  0.00000000e+00, -1.28490008e-07,  0.00000000e+00,
         9.17785774e-09};
    
    vec a2 = { 1.00000000e+00, -3.75618537e+00,  1.49160032e+01, -3.64590663e+01,
         8.62006102e+01, -1.58929766e+02,  2.79121488e+02, -4.14409137e+02,
         5.84695352e+02, -7.23468409e+02,  8.51082988e+02, -8.94437527e+02,
         8.94426960e+02, -8.06151007e+02,  6.91695461e+02, -5.36216565e+02,
         3.95715767e+02, -2.63000210e+02,  1.66317800e+02, -9.38008369e+01,
         5.03045394e+01, -2.35910290e+01,  1.05214957e+01, -3.94854217e+00,
         1.41484447e+00, -3.92253746e-01,  1.05899399e-01, -1.73361510e-02,
         3.06405453e-03};
    y1=energia(b2,a2,x);
    parameters(7)=y1/tot*100;
    return parameters;
}

mat multi_extraction(mat M){
    mat E(8,M.n_cols);
    for (unsigned int i = 0; i < M.n_cols; i++)
    {
        E.col(i)=extraction(M.col(i));
    }
    return E;
}


cv::Mat_<double> to_cvmat(const mat &src)
{
  return cv::Mat_<double>{int(src.n_cols), int(src.n_rows), const_cast<double*>(src.memptr())};
}

void fitScaler(const cv::Mat matriz,cv::Mat *media,cv::Mat *desvio){
    cv::Mat meanValue, stdValue;
    cv::Mat colSTD(1, matriz.rows, CV_64FC1);
    cv::Mat colMEAN(1, matriz.rows, CV_64FC1);       

    for (int i = 0; i < matriz.rows; i++){           
        cv::meanStdDev(matriz.row(i), meanValue, stdValue);
        colSTD.at<double>(i) = stdValue.at<double>(0);
        colMEAN.at<double>(i) = meanValue.at<double>(0);
    }
    *media = colMEAN;
    *desvio = colSTD;
}

cv::Mat transformScaler(const cv::Mat matriz,cv::Mat media,cv::Mat desvio){
    cv::Mat escalonado;
    matriz.copyTo(escalonado);
    for (int i = 0; i < matriz.rows; i++){
        escalonado.row(i)=(escalonado.row(i) - media.at<double>(i))/desvio.at<double>(i);
    }
    return escalonado;
}

int main()
{
    
    mat M_close;
    colvec t_close;
    mat M_open;
    colvec t_open;
    mat M_opened;
    colvec t_opened;

    M_close.load("M_close.csv");
    t_close.load("t_close.csv");
    M_open.load("M_open.csv");
    t_open.load("t_open.csv");
    t_open.fill(1);
    M_opened.load("M_opened.csv");
    t_opened.load("t_opened.csv");
    mat E1 = multi_extraction(M_close);
    mat E2 = multi_extraction(M_opened);
    mat E3 = multi_extraction(M_open);
    mat E = join_rows(E1,E2);
    E = join_rows(E,E3);
    colvec t_total = join_cols(t_close,t_opened);
    t_total = join_cols(t_total,t_open);

    cv::Mat E1_cv = to_cvmat(E1);
    cv::Mat E2_cv = to_cvmat(E2);
    cv::Mat E3_cv = to_cvmat(E3);
    cv::Mat t1 = to_cvmat(t_close);
    cv::Mat t2 = to_cvmat(t_opened);
    cv::Mat t3 = to_cvmat(t_open);
    E1_cv.convertTo(E1_cv,CV_32F);
    E2_cv.convertTo(E2_cv,CV_32F);
    E3_cv.convertTo(E3_cv,CV_32F);
    t1.convertTo(t1,CV_32S);
    t2.convertTo(t2,CV_32S);
    t3.convertTo(t3,CV_32S);
    E1_cv = E1_cv.t();
    E2_cv = E2_cv.t();
    E3_cv = E3_cv.t();
    cv::Ptr<cv::ml::TrainData> E1_data=cv::ml::TrainData::create(E1_cv,1,t1);
    cv::Ptr<cv::ml::TrainData> E2_data=cv::ml::TrainData::create(E2_cv,1,t2);
    cv::Ptr<cv::ml::TrainData> E3_data=cv::ml::TrainData::create(E3_cv,1,t3);
    cv::theRNG().state = time(NULL);
    E1_data->setTrainTestSplitRatio(0.8,true);
    E1_data->shuffleTrainTest ();
    E2_data->setTrainTestSplitRatio(0.8,true);
    E2_data->shuffleTrainTest ();
    E3_data->setTrainTestSplitRatio(0.8,true);
    E3_data->shuffleTrainTest ();
    cv::Mat E_f;
    cv::vconcat(E1_data->getTrainSamples(),E2_data->getTrainSamples(),E_f);
    cv::vconcat(E_f,E3_data->getTrainSamples(),E_f);
    E_f=E_f.t();
    cv::Mat t_f;
    cv::hconcat(E1_data->getTrainResponses(),E2_data->getTrainResponses(),t_f);
    cv::hconcat(t_f,E3_data->getTrainResponses(),t_f);

    cv::Mat E_cv = to_cvmat(E);
    cv::Mat t_cv = to_cvmat(t_total);
    E_cv.convertTo(E_cv,CV_32F);
    t_cv.convertTo(t_cv,CV_32S);
    E_cv=E_cv.t();

    cv::Mat media,desvio;
    fitScaler(E_f,&media,&desvio);
    E_f = transformScaler(E_f,media,desvio);

    cv::FileStorage fs("scaler.yml", FileStorage::WRITE);
    fs << "media" << media;
    fs << "desvio" << desvio;
    fs.release();
    
    cout << "Starting training process" << endl;
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setC(1.0);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, (int)1e7, 1e-6));

    //svm->train(E_cv, COL_SAMPLE, t_cv);
    svm->train(E_f, COL_SAMPLE, t_f);
    svm->save("maquina.xml");
    cout << "Finished training process" << endl;
    cv::Mat E1_idx = E1_data->getTestSampleIdx();
    E1_cv = transformScaler(E1_cv,media,desvio);
    cv::Mat E2_idx = E2_data->getTestSampleIdx();
    E2_cv = transformScaler(E2_cv,media,desvio);
    cv::Mat E3_idx = E3_data->getTestSampleIdx();
    E3_cv = transformScaler(E3_cv,media,desvio);
    int Tn=0;
    int Tp=0;
    int Fn=0;
    int Fp=0;
    for (int i = 0; i < E1_idx.cols; i++)
    {
        Tp += svm->predict(E1_cv.col(E1_idx.at<int>(i)).t());
    }
    for (int i = 0; i < E3_idx.cols; i++)
    {
        Tp += svm->predict(E3_cv.col(E3_idx.at<int>(i)).t());
    }
    Fn = (E1_idx.cols + E3_idx.cols)-Tp;
    for (int i = 0; i < E2_idx.cols; i++)
    {
        Fp += svm->predict(E2_cv.col(E2_idx.at<int>(i)).t());
    }
    Tn = E2_idx.cols - Fp;
    //cv::Mat E4_idx = E1_data->getTestSamples();
    double accuracy = (double)(Tp+Tn) /(double) (Tp+Fp+Tn+Fn);
    double precision = (double)(Tp) /(double) (Tp+Fp);
    double recall = (double)(Tp) / (double)(Tp+Fn);
    double f1_score = 2 * (precision*recall) / (precision+recall);

    cout << "Acuracia: " << accuracy << endl;
    cout << "Precisao: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1_score: " << f1_score << endl;
    cout << "Matriz: " << endl;
    cout << Tp <<" "<< Fn << endl;
    cout << Fp <<" "<< Tn << endl;
    cout << E1_cv.col(5) << endl;

    
    return 0;
}   