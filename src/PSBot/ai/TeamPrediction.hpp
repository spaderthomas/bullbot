// What format does my input data need to be in?
// How do I check multiple classes output?
#if 0

typedef dlib::matrix<int,6,1> input_t;
typedef array<int, 151> output_t;

void build_predictor() {
  MLP<float, float, float> net(6, 151);
  
  Layer<float> input_layer = Layer<float>(6, TanhActivation<float>());
  Layer<float> first_hidden_layer = Layer<float>(100, TanhActivation<float>());
  Layer<float> output_layer = Layer<float>(151, TanhActivation<float>());
  
  net.append_layer(input_layer);
  net.append_layer(first_hidden_layer);
  net.append_layer(output_layer);
  net.finalize(true);
  
  TDLearner<MLP<float, float, float>> agent(net);
  
  dlib::matrix<double,2,1> sample;
  dlib::mlp::kernel_1a_c net(2,5);

  for (int i = 0; i < 1000; ++i) {
    for (int r = -20; r <= 20; ++r) {
      for (int c = -20; c <= 20; ++c) {
        sample(0) = r;
        sample(1) = c;

        // if this point is less than 10 from the origin
        if (sqrt((double)r * r + c * c) <= 10)
          net.train(sample, 1);
        else 
          net.train(sample, 0);
      }
    }
  }

  sample(0) = 3.123;
  sample(1) = 4;
  cout << "This sample should be close to 1 and it is classified as a " << net(sample) << endl;

  sample(0) = 13.123;
  sample(1) = 9.3545;
  cout << "This sample should be close to 0 and it is classified as a " << net(sample) << endl;

  sample(0) = 13.123;
  sample(1) = 0;
  cout << "This sample should be close to 0 and it is classified as a " << net(sample) << endl;
}
#endif

