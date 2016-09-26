#pragma once

// This file is part of the Marian toolkit.
// Marian is copyright (c) 2016 Marcin Junczys-Dowmunt.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>

#include "dataset.h"
#include "batch_generator.h"

namespace marian {
namespace data {

class MNIST : public DataBase {
  private:
    const int IMAGE_MAGIC_NUMBER;
    const int LABEL_MAGIC_NUMBER;

    Examples examples_;

  public:


    MNIST(const std::string& featuresPath,
          const std::string& labelsPath)
    : IMAGE_MAGIC_NUMBER(2051),
      LABEL_MAGIC_NUMBER(2049)
    {
      auto features = ReadImages(featuresPath);
      auto labels = ReadLabels(labelsPath);

      UTIL_THROW_IF2(features.size() != labels.size(),
                     "Features do not match labels");

      for(int i = 0; i < features.size(); ++i)
        examples_.emplace_back(new Example({ features[i], labels[i] }));
    }

    ExampleIterator begin() const {
      return ExampleIterator(examples_.begin());
    }

    ExampleIterator end() const {
      return ExampleIterator(examples_.end());
    }

    void shuffle() {
      std::random_shuffle(examples_.begin(), examples_.end());
    }

  private:
    typedef unsigned char uchar;

    int reverseInt(int i) {
      unsigned char c1, c2, c3, c4;
      c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
      return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    }

    std::vector<DataPtr> ReadImages(const std::string& full_path) {
      std::ifstream file(full_path);
      UTIL_THROW_IF2(!file.is_open(),
                     "Cannot open file `" + full_path + "`!");

      int magic_number = 0;
      file.read((char *)&magic_number, sizeof(magic_number));
      magic_number = reverseInt(magic_number);

      UTIL_THROW_IF2(magic_number != IMAGE_MAGIC_NUMBER,
                     "Invalid MNIST image file!");

      int number_of_images;
      int n_rows = 0;
      int n_cols = 0;

      file.read((char *)&number_of_images, sizeof(number_of_images));
      number_of_images = reverseInt(number_of_images);
      file.read((char *)&n_rows, sizeof(n_rows));
      n_rows = reverseInt(n_rows);
      file.read((char *)&n_cols, sizeof(n_cols));
      n_cols = reverseInt(n_cols);

      int imgSize = n_rows * n_cols;
      std::vector<DataPtr> _dataset(number_of_images);
      for(int i = 0; i < number_of_images; ++i) {
        _dataset[i].reset(new Data(imgSize, 0));
        for (int j = 0; j < imgSize; j++) {
          unsigned char pixel = 0;
          file.read((char*)&pixel, sizeof(pixel));
          (*_dataset[i])[j] = pixel / 255.0f;
        }
      }
      return _dataset;
    }

    std::vector<DataPtr> ReadLabels(const std::string& full_path) {
      std::ifstream file(full_path);

      if (! file.is_open())
        throw std::runtime_error("Cannot open file `" + full_path + "`!");

      int magic_number = 0;
      file.read((char *)&magic_number, sizeof(magic_number));
      magic_number = reverseInt(magic_number);

      if (magic_number != LABEL_MAGIC_NUMBER)
        throw std::runtime_error("Invalid MNIST label file!");

      int number_of_labels;
      file.read((char *)&number_of_labels, sizeof(number_of_labels));
      number_of_labels = reverseInt(number_of_labels);

      std::vector<DataPtr> _dataset(number_of_labels);
      for (int i = 0; i < number_of_labels; i++) {
        _dataset[i].reset(new Data(10, 0.0f));
        unsigned char label;
        file.read((char*)&label, 1);
        (*_dataset[i])[(int)(label)] = 1.0f;
      }

      return _dataset;
    }
};

} // namespace mnist
}
