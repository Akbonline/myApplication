//
// Created by Akshat Bajpai on 7/15/21.
//

#include "NFIQ_2/NFIQ2/NFIQ2Algorithm/include/nfiq2.hpp"
#include "NFIQ_2/NFIQ2/NFIQ2Algorithm/include/nfiq2_algorithm.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "NFIQ2/NFIQ2Algorithm/src/nfiq2/nfiq2_algorithm.cpp"

using namespace std;

// Parses PGM Files. Used to extract data from provided synthetic fingerprint
// images.
void parsePGM(char *filename, std::shared_ptr<uint8_t> &data, uint32_t &rows,
         uint32_t &cols)
{
    // Open PGM file.
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Cannot open image: " << filename << "\n";
        return;
    }

    // Read in magic number.
    std::string magicNumber;
    input >> magicNumber;
    if (magicNumber != "P5") {
        std::cerr << "Error reading magic number from file."
                  << "\n";
        return;
    }

    uint16_t maxValue;
    // Read in image header values - cols, rows.
    // Ignoring the result of maxValue as it is not needed.
    input >> cols >> rows >> maxValue;
    if (!input.good()) {
        std::cerr
                << "Error, premature end of file while reading header."
                << "\n";
        return;
    }
    uint32_t size = cols * rows;

    // Skip line break.
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Allocating new array on heap.
    uint8_t *dataHeap = new uint8_t[size];
    data.reset(dataHeap, std::default_delete<uint8_t[]>());

    // Read in raw pixel data.
    input.read((char *)data.get(), size);
    if (!input.good()) {
        std::cerr << "Error, only read " << input.gcount() << " bytes."
                  << "\n";
        return;
    }
    return;
}

void printUsage()
{
    std::cout << "example_api: usage: example_api [-h] modelInfoFile "
                 "fingerPrintImage\n";
}

void printHelp()
{
    std::cout << "NFIQ 2 API Example\n\nThis is a sample program that "
                 "shows how to use the NFIQ 2 API on an image.\n\nThis "
                 "command line tool takes two arguments.\nThe first is the "
                 "path to a NFIQ 2 RandomForest model information file.\n"
                 "The second is the path to a single fingerprint image.\n\n"
                 "Please provide arguments to the binary in the designated "
                 "order.\nEx: $ example_api nist_plain_tir.txt "
                 "fingerImage01.pgm\n";
}
void run_examples(int argc) {
    const string helpStr{"-h"};
    if (argc == 2 && helpStr.compare("-h") == 0) {   // Add ARGUMENTS HERE
        printHelp();
    }
    static const uint16_t PPI = 500;
    NFIQ2::ModelInfo modelInfoObj{};    // Add ARGUMENTS HERE
    NFIQ2::Algorithm model{};
    modelInfoObj = NFIQ2::ModelInfo("-h");
    model = NFIQ2::Algorithm(modelInfoObj);
    uint32_t rows = 0;
    uint32_t cols = 0;
    shared_ptr<uint8_t> data{};
    char **test = (char **) (char **) "SFinGe_Test01.pgm";  //Image
    parsePGM(*test, data, rows, cols);          // Add ARGUMENTS HERE
    NFIQ2::FingerprintImageData rawImage = NFIQ2::FingerprintImageData(
            data.get(), cols * rows, cols, rows, 0, PPI
    );
    vector<shared_ptr<NFIQ2::QualityFeatures::Module>> modules{};
    modules = NFIQ2::QualityFeatures::computeQualityModules(rawImage);
    unsigned int nfiq2{};
    nfiq2 = model.computeQualityScore(modules);
    cout << "Quality Scores: " << nfiq2 << endl;
    vector<string> actionableIDs = NFIQ2::QualityFeatures::getActionableQualityFeedbackIDs();
    unordered_map<string, double> actionableQuality = NFIQ2::QualityFeatures::getActionableQualityFeedback(
            modules);
    vector<string> featureIDs = NFIQ2::QualityFeatures::getQualityFeatureIDs();
    unordered_map<string, double> qualityFeatures = NFIQ2::QualityFeatures::getQualityFeatureValues(
            modules);
    for (const auto &featureID : featureIDs) {
        cout << featureID << ": " << qualityFeatures.at(featureID) << "\n";
    }

}
