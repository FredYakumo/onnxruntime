// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "gtest/gtest.h"
#include "test/providers/provider_test_utils.h"
#include "core/providers/cpu/tensor/space_depth_ops.h"
#include "core/mlas/inc/mlas.h"

namespace onnxruntime {
namespace test {

template <typename T>
class TensorOpTest : public ::testing::Test {
};

using TensorOpTestTypes = ::testing::Types<float, MLFloat16, uint8_t>;
TYPED_TEST_SUITE(TensorOpTest, TensorOpTestTypes);

TEST(TensorOpTest, SpaceToDepthTest_1) {
  OpTester test("SpaceToDepth");
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  constexpr int64_t N = 1, C = 2, H = 2, W = 4;
  const std::vector<float> X =
      {0.0f, 0.1f, 0.2f, 0.3f,
       1.0f, 1.1f, 1.2f, 1.3f,

       2.0f, 2.1f, 2.2f, 2.3f,
       3.0f, 3.1f, 3.2f, 3.3f};

  test.AddInput<float>("input", {N, C, H, W}, X);

  const std::vector<float> result = {
      0.0f, 0.2f,
      2.0f, 2.2f,

      0.1f, 0.3f,
      2.1f, 2.3f,

      1.0f, 1.2f,
      3.0f, 3.2f,

      1.1f, 1.3f,
      3.1f, 3.3f};
  test.AddOutput<float>("output", {N, C * blocksize * blocksize, H / blocksize, W / blocksize}, result);

  test.Run(OpTester::ExpectResult::kExpectSuccess);
}

TEST(TensorOpTest, SpaceToDepthTest_1_double) {
  OpTester test("SpaceToDepth");
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  constexpr int64_t N = 1, C = 2, H = 2, W = 4;
  const std::vector<double> X =
      {0.0, 0.1, 0.2, 0.3,
       1.0, 1.1, 1.2, 1.3,

       2.0, 2.1, 2.2, 2.3,
       3.0, 3.1, 3.2, 3.3};

  test.AddInput<double>("input", {N, C, H, W}, X);

  const std::vector<double> result = {
      0.0, 0.2,
      2.0, 2.2,

      0.1, 0.3,
      2.1, 2.3,

      1.0, 1.2,
      3.0, 3.2,

      1.1, 1.3,
      3.1, 3.3};

  test.AddOutput<double>("output", {N, C * blocksize * blocksize, H / blocksize, W / blocksize}, result);
  test.Run();
}
TEST(TensorOpTest, SpaceToDepthTest_2) {
  OpTester test("SpaceToDepth");
  constexpr int64_t blocksize = 3;
  test.AddAttribute("blocksize", blocksize);
  constexpr int64_t N = 2, C = 3, H = 3, W = 6;
  const std::vector<float> X = {
      0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.,
      11., 12., 13., 14., 15., 16., 17., 18., 19., 20., 21.,
      22., 23., 24., 25., 26., 27., 28., 29., 30., 31., 32.,
      33., 34., 35., 36., 37., 38., 39., 40., 41., 42., 43.,
      44., 45., 46., 47., 48., 49., 50., 51., 52., 53., 54.,
      55., 56., 57., 58., 59., 60., 61., 62., 63., 64., 65.,
      66., 67., 68., 69., 70., 71., 72., 73., 74., 75., 76.,
      77., 78., 79., 80., 81., 82., 83., 84., 85., 86., 87.,
      88., 89., 90., 91., 92., 93., 94., 95., 96., 97., 98.,
      99., 100., 101., 102., 103., 104., 105., 106., 107.};

  test.AddInput<float>("input", {N, C, H, W}, X);

  const std::vector<float> result = {
      0., 3., 18., 21., 36., 39., 1., 4., 19., 22., 37.,
      40., 2., 5., 20., 23., 38., 41., 6., 9., 24., 27.,
      42., 45., 7., 10., 25., 28., 43., 46., 8., 11., 26.,
      29., 44., 47., 12., 15., 30., 33., 48., 51., 13., 16.,
      31., 34., 49., 52., 14., 17., 32., 35., 50., 53., 54.,
      57., 72., 75., 90., 93., 55., 58., 73., 76., 91., 94.,
      56., 59., 74., 77., 92., 95., 60., 63., 78., 81., 96.,
      99., 61., 64., 79., 82., 97., 100., 62., 65., 80., 83.,
      98., 101., 66., 69., 84., 87., 102., 105., 67., 70., 85.,
      88., 103., 106., 68., 71., 86., 89., 104., 107.};
  test.AddOutput<float>("output", {2, 27, 1, 2}, result);

  test.Run(OpTester::ExpectResult::kExpectSuccess);
}

TEST(TensorOpTest, SpaceToDepthTest_3) {
  // Test swizzling with H_output > 1
  OpTester test("SpaceToDepth");
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  constexpr int64_t N = 1, C = 2, H = 4, W = 8;

  const std::vector<float> X = {
      0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f,
      1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f,

      2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f,
      3.0f, 3.1f, 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f,

      4.0f, 4.1f, 4.2f, 4.3f, 4.4f, 4.5f, 4.6f, 4.7f,
      5.0f, 5.1f, 5.2f, 5.3f, 5.4f, 5.5f, 5.6f, 5.7f,
      6.0f, 6.1f, 6.2f, 6.3f, 6.4f, 6.5f, 6.6f, 6.7f,
      7.0f, 7.1f, 7.2f, 7.3f, 7.4f, 7.5f, 7.6f, 7.7f};

  test.AddInput<float>("input", {N, C, H, W}, X);

  const std::vector<float> result = {
      0.0f, 0.2f, 0.4f, 0.6f,
      2.0f, 2.2f, 2.4f, 2.6f,
      4.0f, 4.2f, 4.4f, 4.6f,
      6.0f, 6.2f, 6.4f, 6.6f,

      0.1f, 0.3f, 0.5f, 0.7f,
      2.1f, 2.3f, 2.5f, 2.7f,
      4.1f, 4.3f, 4.5f, 4.7f,
      6.1f, 6.3f, 6.5f, 6.7f,

      1.0f, 1.2f, 1.4f, 1.6f,
      3.0f, 3.2f, 3.4f, 3.6f,
      5.0f, 5.2f, 5.4f, 5.6f,
      7.0f, 7.2f, 7.4f, 7.6f,

      1.1f, 1.3f, 1.5f, 1.7f,
      3.1f, 3.3f, 3.5f, 3.7f,
      5.1f, 5.3f, 5.5f, 5.7f,
      7.1f, 7.3f, 7.5f, 7.7f};

  test.AddOutput<float>("output", {N, C * blocksize * blocksize, H / blocksize, W / blocksize}, result);

  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_1) {
  OpTester test("DepthToSpace", 7);  // create an opset 7 model
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 1, C = 8, H = 1, W = 2;
  const std::vector<float> X = {
      0.0f, 0.2f,
      2.0f, 2.2f,

      0.1f, 0.3f,
      2.1f, 2.3f,

      1.0f, 1.2f,
      3.0f, 3.2f,

      1.1f, 1.3f,
      3.1f, 3.3f};

  test.AddInput<float>("input", {N, C, H, W}, X);

  const std::vector<float> result =
      {0.0f, 0.1f, 0.2f, 0.3f,
       1.0f, 1.1f, 1.2f, 1.3f,

       2.0f, 2.1f, 2.2f, 2.3f,
       3.0f, 3.1f, 3.2f, 3.3f};
  test.AddOutput<float>("output", {N, C / (blocksize * blocksize), H * blocksize, W * blocksize}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_1_double) {
  OpTester test("DepthToSpace", 7);  // create an opset 7 model
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 1, C = 8, H = 1, W = 2;
  const std::vector<double> X = {
      0.0, 0.2,
      2.0, 2.2,

      0.1, 0.3,
      2.1, 2.3,

      1.0, 1.2,
      3.0, 3.2,

      1.1, 1.3,
      3.1, 3.3};

  test.AddInput<double>("input", {N, C, H, W}, X);

  const std::vector<double> result =
      {0.0, 0.1, 0.2, 0.3,
       1.0, 1.1, 1.2, 1.3,

       2.0, 2.1, 2.2, 2.3,
       3.0, 3.1, 3.2, 3.3};
  test.AddOutput<double>("output", {N, C / (blocksize * blocksize), H * blocksize, W * blocksize}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_2) {
  OpTester test("DepthToSpace", 7);  // create an opset 7 model
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 2, C = 12, H = 3, W = 2;
  const std::vector<float> X = {
      0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.,
      11., 12., 13., 14., 15., 16., 17., 18., 19., 20., 21.,
      22., 23., 24., 25., 26., 27., 28., 29., 30., 31., 32.,
      33., 34., 35., 36., 37., 38., 39., 40., 41., 42., 43.,
      44., 45., 46., 47., 48., 49., 50., 51., 52., 53., 54.,
      55., 56., 57., 58., 59., 60., 61., 62., 63., 64., 65.,
      66., 67., 68., 69., 70., 71., 72., 73., 74., 75., 76.,
      77., 78., 79., 80., 81., 82., 83., 84., 85., 86., 87.,
      88., 89., 90., 91., 92., 93., 94., 95., 96., 97., 98.,
      99., 100., 101., 102., 103., 104., 105., 106., 107., 108., 109.,
      110., 111., 112., 113., 114., 115., 116., 117., 118., 119., 120.,
      121., 122., 123., 124., 125., 126., 127., 128., 129., 130., 131.,
      132., 133., 134., 135., 136., 137., 138., 139., 140., 141., 142.,
      143.};

  test.AddInput<float>("input", {N, C, H, W}, X);

  const std::vector<float> result = {
      0., 18., 1., 19., 36., 54., 37., 55., 2., 20., 3.,
      21., 38., 56., 39., 57., 4., 22., 5., 23., 40., 58.,
      41., 59., 6., 24., 7., 25., 42., 60., 43., 61., 8.,
      26., 9., 27., 44., 62., 45., 63., 10., 28., 11., 29.,
      46., 64., 47., 65., 12., 30., 13., 31., 48., 66., 49.,
      67., 14., 32., 15., 33., 50., 68., 51., 69., 16., 34.,
      17., 35., 52., 70., 53., 71., 72., 90., 73., 91., 108.,
      126., 109., 127., 74., 92., 75., 93., 110., 128., 111., 129.,
      76., 94., 77., 95., 112., 130., 113., 131., 78., 96., 79.,
      97., 114., 132., 115., 133., 80., 98., 81., 99., 116., 134.,
      117., 135., 82., 100., 83., 101., 118., 136., 119., 137., 84.,
      102., 85., 103., 120., 138., 121., 139., 86., 104., 87., 105.,
      122., 140., 123., 141., 88., 106., 89., 107., 124., 142., 125.,
      143.};
  test.AddOutput<float>("output", {2, 3, 6, 4}, result);
  test.Run();
}

TYPED_TEST(TensorOpTest, DepthToSpaceTest_3) {
  OpTester test("DepthToSpace", 11);  // create an opset 11 model with missing default attribute
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 2, C = 12, H = 3, W = 2;
  const std::vector<float> X = {
      0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.,
      11., 12., 13., 14., 15., 16., 17., 18., 19., 20., 21.,
      22., 23., 24., 25., 26., 27., 28., 29., 30., 31., 32.,
      33., 34., 35., 36., 37., 38., 39., 40., 41., 42., 43.,
      44., 45., 46., 47., 48., 49., 50., 51., 52., 53., 54.,
      55., 56., 57., 58., 59., 60., 61., 62., 63., 64., 65.,
      66., 67., 68., 69., 70., 71., 72., 73., 74., 75., 76.,
      77., 78., 79., 80., 81., 82., 83., 84., 85., 86., 87.,
      88., 89., 90., 91., 92., 93., 94., 95., 96., 97., 98.,
      99., 100., 101., 102., 103., 104., 105., 106., 107., 108., 109.,
      110., 111., 112., 113., 114., 115., 116., 117., 118., 119., 120.,
      121., 122., 123., 124., 125., 126., 127., 128., 129., 130., 131.,
      132., 133., 134., 135., 136., 137., 138., 139., 140., 141., 142.,
      143.};

  const std::vector<float> result = {
      0., 18., 1., 19., 36., 54., 37., 55., 2., 20., 3.,
      21., 38., 56., 39., 57., 4., 22., 5., 23., 40., 58.,
      41., 59., 6., 24., 7., 25., 42., 60., 43., 61., 8.,
      26., 9., 27., 44., 62., 45., 63., 10., 28., 11., 29.,
      46., 64., 47., 65., 12., 30., 13., 31., 48., 66., 49.,
      67., 14., 32., 15., 33., 50., 68., 51., 69., 16., 34.,
      17., 35., 52., 70., 53., 71., 72., 90., 73., 91., 108.,
      126., 109., 127., 74., 92., 75., 93., 110., 128., 111., 129.,
      76., 94., 77., 95., 112., 130., 113., 131., 78., 96., 79.,
      97., 114., 132., 115., 133., 80., 98., 81., 99., 116., 134.,
      117., 135., 82., 100., 83., 101., 118., 136., 119., 137., 84.,
      102., 85., 103., 120., 138., 121., 139., 86., 104., 87., 105.,
      122., 140., 123., 141., 88., 106., 89., 107., 124., 142., 125.,
      143.};

  if constexpr (std::is_same<TypeParam, float>::value) {
    test.AddInput<float>("input", {N, C, H, W}, X);
    test.AddOutput<float>("output", {2, 3, 6, 4}, result);
  } else if constexpr (std::is_same<TypeParam, MLFloat16>::value) {
    std::vector<TypeParam> X_fp16(X.size());
    std::vector<TypeParam> result_fp16(result.size());
    ConvertFloatToMLFloat16(X.data(), X_fp16.data(), X.size());
    ConvertFloatToMLFloat16(result.data(), result_fp16.data(), result.size());
    test.AddInput<TypeParam>("input", {N, C, H, W}, X_fp16);
    test.AddOutput<TypeParam>("output", {2, 3, 6, 4}, result_fp16);
  } else if constexpr (std::is_same<TypeParam, uint8_t>::value) {
    std::vector<uint8_t> X_u8(X.size());
    std::vector<uint8_t> result_u8(result.size());
    ConvertFloatToUint8_t(X.data(), X_u8.data(), X.size());
    ConvertFloatToUint8_t(result.data(), result_u8.data(), result.size());
    test.AddInput<uint8_t>("input", {N, C, H, W}, X_u8);
    test.AddOutput<uint8_t>("output", {2, 3, 6, 4}, result_u8);
  } else {
    ORT_THROW("Type not supported");
  }

  // type not supported by QNN EP: MLFloat16 and unsigned char
  test.Run(OpTester::ExpectResult::kExpectSuccess, "", {kQnnExecutionProvider});
}

TYPED_TEST(TensorOpTest, DepthToSpaceTest_4) {
  OpTester test("DepthToSpace", 11);  // create an opset 11 model with attribute present = "DCR" mode
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "DCR");

  constexpr int64_t N = 2, C = 12, H = 3, W = 2;
  const std::vector<float> X = {
      0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.,
      11., 12., 13., 14., 15., 16., 17., 18., 19., 20., 21.,
      22., 23., 24., 25., 26., 27., 28., 29., 30., 31., 32.,
      33., 34., 35., 36., 37., 38., 39., 40., 41., 42., 43.,
      44., 45., 46., 47., 48., 49., 50., 51., 52., 53., 54.,
      55., 56., 57., 58., 59., 60., 61., 62., 63., 64., 65.,
      66., 67., 68., 69., 70., 71., 72., 73., 74., 75., 76.,
      77., 78., 79., 80., 81., 82., 83., 84., 85., 86., 87.,
      88., 89., 90., 91., 92., 93., 94., 95., 96., 97., 98.,
      99., 100., 101., 102., 103., 104., 105., 106., 107., 108., 109.,
      110., 111., 112., 113., 114., 115., 116., 117., 118., 119., 120.,
      121., 122., 123., 124., 125., 126., 127., 128., 129., 130., 131.,
      132., 133., 134., 135., 136., 137., 138., 139., 140., 141., 142.,
      143.};

  const std::vector<float> result = {
      0., 18., 1., 19., 36., 54., 37., 55., 2., 20., 3.,
      21., 38., 56., 39., 57., 4., 22., 5., 23., 40., 58.,
      41., 59., 6., 24., 7., 25., 42., 60., 43., 61., 8.,
      26., 9., 27., 44., 62., 45., 63., 10., 28., 11., 29.,
      46., 64., 47., 65., 12., 30., 13., 31., 48., 66., 49.,
      67., 14., 32., 15., 33., 50., 68., 51., 69., 16., 34.,
      17., 35., 52., 70., 53., 71., 72., 90., 73., 91., 108.,
      126., 109., 127., 74., 92., 75., 93., 110., 128., 111., 129.,
      76., 94., 77., 95., 112., 130., 113., 131., 78., 96., 79.,
      97., 114., 132., 115., 133., 80., 98., 81., 99., 116., 134.,
      117., 135., 82., 100., 83., 101., 118., 136., 119., 137., 84.,
      102., 85., 103., 120., 138., 121., 139., 86., 104., 87., 105.,
      122., 140., 123., 141., 88., 106., 89., 107., 124., 142., 125.,
      143.};

  if constexpr (std::is_same<TypeParam, float>::value) {
    test.AddInput<float>("input", {N, C, H, W}, X);
    test.AddOutput<float>("output", {2, 3, 6, 4}, result);
  } else if constexpr (std::is_same<TypeParam, MLFloat16>::value) {
    std::vector<TypeParam> X_fp16(X.size());
    std::vector<TypeParam> result_fp16(result.size());
    ConvertFloatToMLFloat16(X.data(), X_fp16.data(), X.size());
    ConvertFloatToMLFloat16(result.data(), result_fp16.data(), result.size());
    test.AddInput<TypeParam>("input", {N, C, H, W}, X_fp16);
    test.AddOutput<TypeParam>("output", {2, 3, 6, 4}, result_fp16);
  } else if constexpr (std::is_same<TypeParam, uint8_t>::value) {
    std::vector<uint8_t> X_u8(X.size());
    std::vector<uint8_t> result_u8(result.size());
    ConvertFloatToUint8_t(X.data(), X_u8.data(), X.size());
    ConvertFloatToUint8_t(result.data(), result_u8.data(), result.size());
    test.AddInput<uint8_t>("input", {N, C, H, W}, X_u8);
    test.AddOutput<uint8_t>("output", {2, 3, 6, 4}, result_u8);
  } else {
    ORT_THROW("Type not supported");
  }

  // type not supported by QNN EP: MLFloat16 and unsigned char
  test.Run(OpTester::ExpectResult::kExpectSuccess, "", {kQnnExecutionProvider});
}

TYPED_TEST(TensorOpTest, DepthToSpaceTest_5) {
  OpTester test("DepthToSpace", 11);  // create an opset 11 model with attribute present = "CRD" mode
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "CRD");

  constexpr int64_t N = 1, C = 4, H = 2, W = 3;
  const std::vector<float> X = {0., 1., 2.,
                                3., 4., 5.,
                                9., 10., 11.,
                                12., 13., 14.,
                                18., 19., 20.,
                                21., 22., 23.,
                                27., 28., 29.,
                                30., 31., 32.};

  const std::vector<float> result = {0., 9., 1., 10., 2., 11.,
                                     18., 27., 19., 28., 20., 29.,
                                     3., 12., 4., 13., 5., 14.,
                                     21., 30., 22., 31., 23., 32.};

  if constexpr (std::is_same<TypeParam, float>::value) {
    test.AddInput<float>("input", {N, C, H, W}, X);
    test.AddOutput<float>("output", {1, 1, 4, 6}, result);
  } else if constexpr (std::is_same<TypeParam, MLFloat16>::value) {
    std::vector<TypeParam> X_fp16(X.size());
    std::vector<TypeParam> result_fp16(result.size());
    ConvertFloatToMLFloat16(X.data(), X_fp16.data(), X.size());
    ConvertFloatToMLFloat16(result.data(), result_fp16.data(), result.size());
    test.AddInput<TypeParam>("input", {N, C, H, W}, X_fp16);
    test.AddOutput<TypeParam>("output", {1, 1, 4, 6}, result_fp16);
  } else if constexpr (std::is_same<TypeParam, uint8_t>::value) {
    std::vector<uint8_t> X_u8(X.size());
    std::vector<uint8_t> result_u8(result.size());
    ConvertFloatToUint8_t(X.data(), X_u8.data(), X.size());
    ConvertFloatToUint8_t(result.data(), result_u8.data(), result.size());
    test.AddInput<uint8_t>("input", {N, C, H, W}, X_u8);
    test.AddOutput<uint8_t>("output", {1, 1, 4, 6}, result_u8);
  } else {
    ORT_THROW("Type not supported");
  }

  // type not supported by QNN EP: MLFloat16 and unsigned char
  test.Run(OpTester::ExpectResult::kExpectSuccess, "", {kQnnExecutionProvider});
}

TEST(TensorOpTest, DepthToSpaceTest_CRD_Batched) {
  OpTester test("DepthToSpace", 11);  // create an opset 11 model with attribute present = "CRD" mode
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "CRD");

  constexpr int64_t N = 2, C = 4, H = 2, W = 3;
  std::vector<float> X = {0., 1., 2.,
                          3., 4., 5.,
                          9., 10., 11.,
                          12., 13., 14.,
                          18., 19., 20.,
                          21., 22., 23.,
                          27., 28., 29.,
                          30., 31., 32.};

  // append same data but in reverse order so we can tell if the batch output is wrong
  X.insert(X.end(), X.rbegin(), X.rend());

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0., 9., 1., 10., 2., 11.,
                               18., 27., 19., 28., 20., 29.,
                               3., 12., 4., 13., 5., 14.,
                               21., 30., 22., 31., 23., 32.};
  result.insert(result.end(), result.rbegin(), result.rend());

  test.AddOutput<float>("output", {2, 1, 4, 6}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_DefaultMode1) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 1, C = 8, H = 1, W = 1;
  std::vector<float> X = {0, 9, 18, 27, 36, 45, 54, 63};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 18, 36, 54, 9, 27, 45, 63};

  test.AddOutput<float>("output", {1, 2, 2, 2}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_DefaultMode2) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);

  constexpr int64_t N = 2, C = 8, H = 1, W = 2;
  std::vector<float> X = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
                          29, 30, 31};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 4, 1, 5, 8, 12, 9, 13, 2, 6, 3, 7, 10, 14, 11, 15, 16, 20, 17, 21, 24, 28, 25, 29, 18, 22, 19, 23, 26,
                               30, 27, 31};

  test.AddOutput<float>("output", {2, 2, 2, 4}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_DCR1) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "DCR");

  constexpr int64_t N = 1, C = 8, H = 1, W = 1;
  std::vector<float> X = {0, 9, 18, 27, 36, 45, 54, 63};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 18, 36, 54, 9, 27, 45, 63};

  test.AddOutput<float>("output", {1, 2, 2, 2}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_DCR2) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "DCR");

  constexpr int64_t N = 2, C = 8, H = 1, W = 2;
  std::vector<float> X = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
                          29, 30, 31};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 4, 1, 5, 8, 12, 9, 13, 2, 6, 3, 7, 10, 14, 11, 15, 16, 20, 17, 21, 24, 28, 25, 29, 18, 22, 19, 23, 26,
                               30, 27, 31};

  test.AddOutput<float>("output", {2, 2, 2, 4}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_CRD1) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "CRD");

  constexpr int64_t N = 1, C = 8, H = 1, W = 1;
  std::vector<float> X = {0, 9, 18, 27, 36, 45, 54, 63};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 9, 18, 27, 36, 45, 54, 63};

  test.AddOutput<float>("output", {1, 2, 2, 2}, result);
  test.Run();
}

TEST(TensorOpTest, DepthToSpaceTest_WebGPU_CRD2) {
  OpTester test("DepthToSpace", 11);
  constexpr int64_t blocksize = 2;
  test.AddAttribute("blocksize", blocksize);
  test.AddAttribute("mode", "CRD");

  constexpr int64_t N = 2, C = 8, H = 1, W = 2;
  std::vector<float> X = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
                          29, 30, 31};

  test.AddInput<float>("input", {N, C, H, W}, X);

  std::vector<float> result = {0, 2, 1, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 14, 13, 15, 16, 18, 17, 19, 20, 22, 21, 23, 24, 26, 25, 27, 28,
                               30, 29, 31};

  test.AddOutput<float>("output", {2, 2, 2, 4}, result);
  test.Run();
}

}  // namespace test
}  // namespace onnxruntime
