/*
 * Proprietary License
 *
 * Copyright (c) 2024-2025 Dean S Horak
 * All rights reserved.
 *
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 *
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include "HTTPServerMain.h"


// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern double DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;

unsigned char bitmapper[8] = {128, 64, 32, 16, 8, 4, 2, 1};
// unsigned char andbitmapper[8] = {127, 191, 223, 239, 247, 251, 253, 254};

typedef unsigned char BYTE;

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

int randomFireCount = 0;


static inline bool is_base64(BYTE c)
{
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encoder(BYTE const *buf, unsigned int bufLen)
{
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--)
  {
    char_array_3[i++] = *(buf++);
    if (i == 3)
    {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}

std::vector<BYTE> base64_decoder(std::string const &encoded_string)
{
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  BYTE char_array_4[4], char_array_3[3];
  std::vector<BYTE> ret;

  while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
  {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4)
    {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++)
      ret.push_back(char_array_3[j]);
  }

  return ret;
}

long setActivationPatternCount = 0;

std::string base64_encode(unsigned char const *bytes_to_encode, size_t in_len)
{

  const BYTE *buf = bytes_to_encode;
  unsigned int buflen = (unsigned int)in_len;
  std::string result;
  result = base64_encoder(buf, buflen);
  return result;
}

unsigned char *base64_decode(std::string const &encoded_string, size_t *out_len)
{
  const std::string s = encoded_string;

  std::vector<BYTE> result = base64_decoder(s);
  unsigned char *ret = (unsigned char *)calloc(1, result.size());
  size_t resultsSize = result.size();
  for (size_t i = 0; i < resultsSize; i++)
  {
    ret[i] = result[i];
  }
  *out_len = resultsSize;
  return ret;
}

void ensure_enough_resource(int resource, std::uint32_t soft_limit,
                            std::uint32_t hard_limit)
{
  rlimit new_limit, old_limit;

  new_limit.rlim_cur = soft_limit;
  new_limit.rlim_max = hard_limit;
  getrlimit(resource, &old_limit);

  std::cout << "Old limit: " << old_limit.rlim_cur << " (soft limit), "
            << old_limit.rlim_cur << " (hard limit)." << std::endl;
  std::cout << "New limit: " << new_limit.rlim_cur << " (soft limit), "
            << new_limit.rlim_cur << " (hard limit)." << std::endl;

  if (setrlimit(resource, &new_limit))
  {
    std::cerr << "Warning: Could not update resource limit ";
    std::cerr << "(" << strerror(errno) << ")." << std::endl;
    std::cerr << "Consider setting the limit manually with ulimit" << std::endl;
    exit(-1);
  }
}

size_t min(size_t a, size_t b)
{
  return (a < b) ? a : b;
}

// Function to convert the grayscale pixel value to an ASCII character
char pixelToAscii(unsigned char pixelValue)
{
  // Expanded set of ASCII characters for 50 levels of brightness
  const char *asciiChars = " .`'^\",-~:;_+=<>i!lI?/\\|()1{}[]rctvzXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
  int numLevels = 50; // Number of levels of brightness represented by ASCII chars

  // Map the pixel value (0-255) to an index in the ASCII character array (0-49)
  int index = (pixelValue * (numLevels - 1)) / 255;
  return asciiChars[index];
}

// Function to display the 28x28 grayscale image buffer as ASCII art
void displayAsciiImage(unsigned char *tempBuffer)
{
  if (!tempBuffer)
  {
    std::cerr << "Error: Buffer is null." << std::endl;
    return;
  }

  std::stringstream pattern;
  for (int row = 0; row < 28; ++row)
  {
    for (int col = 0; col < 28; ++col)
    {
      // Get the pixel value from the buffer
      unsigned char pixelValue = tempBuffer[row * 28 + col];
      // Convert the pixel value to an ASCII character and print it
      pattern << pixelToAscii(pixelValue);
    }
    // Move to the next line after printing each row
    pattern << std::endl;
  }

  globalObject->lastImageString = pattern.str().c_str();
  globalObject->writeFirePatternLog(globalObject->lastImageString);
}

std::string parseAndRespond(std::string content)
{
  (void)content;
  // get number of APs

  std::stringstream response;

  // size_t apCount = 0;
  // for (typename std::map<long, TimedEvent*>::iterator timedEventIterator = globalObject->allTimedEvents.begin(); timedEventIterator != globalObject->allTimedEvents.end(); ++timedEventIterator)
  //{
  // TimedEvent* te = timedEventIterator->second;
  // ActionPotential *ap = te->ap;
  //      apCount++;
  //}

  //	std::vector<TimedEvent*>* teVector = globalObject->getTimedEventCollection();

  response << "<h2>Timed Events</h2>";

  response << "<p>" << globalObject->allTimedEvents.size() << " total timed events" << "<p>";

  for (size_t i = 0; i < MAX_TIMEINTERVAL_BUFFER_SIZE; i++)
  {
    size_t intervalOffsetValue = (globalObject->getCurrentTimestamp() + i) % MAX_TIMEINTERVAL_BUFFER_SIZE;
    std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[intervalOffsetValue];
    long sz = teVector->size();

    if (sz > 0)
      response << "<p>Interval " << i << ": " << sz << "<p>";
  }

  return response.str();
}

std::string bitsToString(unsigned char *buffer, long numbits)
{
  std::string result;
  for (size_t index = 0; index < (size_t)numbits; index++)
  {
    size_t byteIndex = index / 8;
    size_t bitIndex = index % 8;
    unsigned char thisbyte = buffer[byteIndex] & bitmapper[bitIndex];
    if (thisbyte == '\0')
      result += "0";
    else
      result += "1";
  }

  return result;
}

std::string parseAndRespondJSON(std::string content)
{

  //  std::cout << content << std::endl;

  nlohmann::json jsonObject = nlohmann::json::parse(content);

  std::string command = jsonObject["command"];

  int returnCode = 999;
  std::string returnMessage = "Unknown command received: " + command;

  if (!command.empty())
  {
    if (boost::iequals(command, "GETNEURONS"))
    {
      //
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
    }
    else if (boost::iequals(command, "SETACTIVATION"))
    {
      //
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
    }
    else if (boost::iequals(command, "GETACTIVATION"))
    {
      //
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
    }
    else if (boost::iequals(command, "GETACTIVENEURONS"))
    {
      //
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
    }
    else if (boost::iequals(command, "STARTEVENTLOG"))
    {
      //
      globalObject->logEvents = true;
      returnCode = 0;
      returnMessage = "EVENT LOGGING ON ";

      std::string returnString("");
      returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "STOPEVENTLOG"))
    {
      //
      globalObject->closeEventLog();
      globalObject->logEvents = false;
      returnCode = 0;
      returnMessage = "EVENT LOGGING OFF ";

      std::string returnString("");
      returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "LOGRESPONSEON"))
    {
      //
      globalObject->closeEventLog();
      globalObject->logResponseMode = true;
      returnCode = 0;
      returnMessage = "LOGRESPONSE ON ";

      std::string returnString("");
      returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "LOGRESPONSEOFF"))
    {
      //
      globalObject->closeEventLog();
      globalObject->logResponseMode = false;
      returnCode = 0;
      returnMessage = "LOGRESPONSE OFF ";

      std::string returnString("");
      returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "GETTESTRESPONSE"))
    {

      // globalObject->logResponseMode = true;

      auto nuclei = jsonObject["nuclei"];
      if (nuclei.is_array())
      {
        auto inputElement = nuclei[0];
        auto outputElement = nuclei[1];

        std::string inputNucleus = inputElement["nucleusin"];
        std::string inputPattern = inputElement["pattern"];

        std::string outputNucleus = outputElement["nucleusout"];

        std::vector<long> neurons = Server::getNeurons(inputNucleus, LayerType::input); // Layer 1 = input
        size_t totalSize = neurons.size();

        size_t tempBufferSize = 0;
        unsigned char *tempBuffer = base64_decode(inputPattern, &tempBufferSize);

        if(tempBufferSize == (28*28)) {
          displayAsciiImage(tempBuffer);
          memcpy(globalObject->lastBuffer,tempBuffer,28*28);
        }

        size_t numbits = min(totalSize, tempBufferSize * 8);

        std::vector<Neuron *> firingNeurons;
        // We now have the vector of neurons to update and the bit pattern to update them with
        for (int i = 0; i < (int)numbits; i++)
        {
          int byteindex = i / 8;
          int bitindex = i % 8; // get the modulus
          long neuronId = neurons[i];
          Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);

          unsigned char value = *(tempBuffer + byteindex); // get value of byte
          value &= bitmapper[bitindex];

          if (value != 0)
          { // Turn on firing
            // neuron->potential=1.0; // GETTESTRESPONSE is always 1
            // neuron->fire();
            firingNeurons.push_back(neuron);
          }
        }

        // std::string thispattern = bitsToString(tempBuffer, numbits);
        //        std::string thispattern = convertToBinaryString(tempBuffer, tempBufferSize);

        //        std::cout << inputNucleus << " pattern: " << thispattern << std::endl;

        //
        free(tempBuffer);

        long defaultWait = 1020;
        Range offset = globalObject->batchFire(&firingNeurons, defaultWait);
        long lowestOffset = offset.low;
        long highestOffset = offset.high;
        long startTimer = globalObject->getCurrentTimestamp();

        long delay = PROPAGATION_DELAY_MICROSECONDS;
        if (highestOffset < delay && highestOffset > 0)
          delay = highestOffset;

        if (delay > MAX_TIMEINTERVAL_OFFSET)
          delay = MAX_TIMEINTERVAL_OFFSET;

        long endTimer = startTimer + delay + PROPAGATION_DELAY_MICROSECONDS;
        // globalObject->cycleNeurons(); // cycle through all neurons a if potiential > threshold, fire the neurons

        /*
                while(globalObject->getCurrentTimestamp() < endTimer)
                {
        //          std::cout << "delay current: " << glofiringNeuronsbalObject->getCurrentTimestamp() << ", till: " << endTimer << std::endl;
                  usleep(10); // 20ms delay to allow for ap propagation
                }
        */
        // we have fired the stimulus. Lets give a few ms to allow the APs to propagate.
         //usleep(delay + PROPAGATION_DELAY_MICROSECONDS); // 5ms which is half the refactor period

        // Now lets return the results from the output neurons

        // std::string pattern("");
        std::string nucleus = outputNucleus;
        if (nucleus.empty())
        {
          returnCode = 500; // invalid object reference
          returnMessage = "invalid object reference";
          std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
          return returnString;
        }

        int totalFirings = 0;
        float highestWeight = -65.0f;
        Neuron *highestNeuron = NULL;
        neurons = Server::getNeurons(nucleus, LayerType::output); //  output
        totalSize = neurons.size();

        tempBufferSize = totalSize / 8 + 1;

        tempBuffer = (unsigned char *)calloc(1, tempBufferSize);


        // wait for the first neuron firing, or 
        long endTime = startTimer+ delay + PROPAGATION_DELAY_MICROSECONDS;
        bool didFire = false;
        long ct = globalObject->getCurrentTimestamp();
        // wait for a response
        endTime +=25; // wait max of 0.025 seconds times as longer than expected
        bool done=false;
        while (!done)
        {
          for (size_t i = 0; i < totalSize; i++)
          {
            if(!didFire)
            {
              ct = globalObject->getCurrentTimestamp();
              long neuronId = neurons[i];
              Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
              if (neuron->isFiring(FIRING_WINDOW))
              {
//                std::cout << "Neuron " << neuron->id << " firing after " << (ct - startTimer) << "ms" << std::endl;
                didFire = true;
              }
            }
            else
            {
              break;
            }
          }
          if(ct >= endTime || didFire)
            done = true;
          else 
            usleep(25);
        }
/*
        if(!didFire) {
          long ct = globalObject->getCurrentTimestamp();
          std::cout << "Giving up waiting on firing after " << (ct - startTimer) << "ms" << std::endl;
          std::cout << "StarTimer: " << startTimer << "ms" << " Current Time:" << ct << " EndTime: " << endTime << std::endl;
        }
*/

        for (size_t i = 0; i < totalSize; i++)
        {
          long neuronId = neurons[i];
          Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
          int byteindex = i / 8;
          int bitindex = i % 8; // get the modulus

          if (neuron != NULL)
          {
            if (neuron->isFiring(FIRING_WINDOW)) // Allow for a 10 ms window to be considered "firing"
            {
              totalFirings++;
              tempBuffer[byteindex] = tempBuffer[byteindex] | bitmapper[bitindex];
            }
          }
        }
        //        }
        // tempBuffer now contains the activations in binary (on bit indicates neuron is firing)

        // printf("Response: %s\n",convertBitmapToString(tempBuffer,totalSize).c_str());
        // convert the buffer into a base64 string

        // thispattern = bitsToString(tempBuffer, totalSize);

        // std::cout << outputNucleus << " pattern: " << thispattern << std::endl;

        std::string encoded = base64_encode(tempBuffer, tempBufferSize);
        // release the tempbuffer
        free(tempBuffer);

        // format the return json string

        std::stringstream st;

        st << "SUCCESS TotalFirings = " << totalFirings;

        returnCode = 0;
        returnMessage = st.str();

        //        if (totalFirings > 0)
        //          printf("Total Firings = %d\n", totalFirings);

        std::string returnString("{ \"pattern\": \"");
        returnString += encoded;
        returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        // printf("%s\n",returnString.c_str());
        return returnString;
      }
      else
      {
        returnCode = 997;
        returnMessage = "nuclei missing: " + command;
        std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        // printf("%s\n",returnString.c_str());
        return returnString;
      }
    }
    else if (boost::iequals(command, "SETACTIVATIONPATTERN"))
    {

      globalObject->logResponseMode = false;

      setActivationPatternCount++;
      //      printf("SETACTIVATIONPATTERN %d\r",setActivationPatternCount);
      // std::cout << "SETACTIVATIONPATTERN " << setActivationPatternCount << std::endl;

      //      int retryCount = 0;
      std::vector<Neuron *> firingNeurons;
      std::vector<Neuron *> inputFiringNeurons;
      std::vector<Neuron *> outputFiringNeurons;
      std::string nucleusName;

      // bool firing = false;
      //       while (!firing && retryCount < 100)
      //       {
      firingNeurons.clear();
      inputFiringNeurons.clear();
      outputFiringNeurons.clear();
      auto nuclei = jsonObject["nuclei"];
      if (nuclei.is_array())
      {
        int nucleusIndex = 0;
        for (const auto &element : nuclei)
        {

          std::string nucleus = element["nucleus"];
          nucleusName = nucleus;

          // printf("processing nucleus %s\n", nucleus.c_str());

          std::string pattern = element["pattern"];

          std::vector<long> neurons;
          if (nucleusIndex == 0)
            neurons = Server::getNeurons(nucleus, LayerType::input); // Layer input
          else
            neurons = Server::getNeurons(nucleus, LayerType::output); // Layer output

          size_t totalSize = neurons.size();

          size_t tempBufferSize = 0;
          unsigned char *tempBuffer = base64_decode(pattern, &tempBufferSize);

          size_t numbits = min(totalSize, tempBufferSize * 8);

          if (nucleusIndex == 0) {
            displayAsciiImage(tempBuffer);
            memcpy(globalObject->lastBuffer,tempBuffer,28*28);
          }

          //          std::string thispattern = bitsToString(tempBuffer, numbits);

          //          std::cout << nucleus << " pattern: " << thispattern << std::endl;

          // We now have the vector of neurons to update and the bit pattern to update them with
          std::string outString;
          for (int i = 0; i < (int)numbits; i++)
          {
            int byteindex = i / 8;
            int bitindex = i % 8; // get the modulus
            long neuronId = neurons[i];
            Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);

            unsigned char value = tempBuffer[byteindex]; // get value of byte
            value = value & bitmapper[bitindex];

            if (value != 0)
            { // Turn on firing
              firingNeurons.push_back(neuron);

              if (nucleusIndex == 0) // input
              {
                inputFiringNeurons.push_back(neuron);
              }
              else
              { // output
                outString += "1";
                outputFiringNeurons.push_back(neuron);
              }
            }
            else
            {
              if (nucleusIndex != 0) // input
              {
                outString += "0";
              }
            }
          }

          globalObject->latestOutputTarget = outString;

          //
          free(tempBuffer);

          nucleusIndex++;
        }

        bool growDendrites = GROW_DENDRITES;

        if (growDendrites)
        {
          // experimental growing dendrites.
          size_t infNeuronsCount = inputFiringNeurons.size();
          size_t outfNeuronsCount = outputFiringNeurons.size();
          for (size_t i = 0; i < infNeuronsCount; i++)
          {
            Neuron *in = inputFiringNeurons[i];
            for (size_t j = 0; j < outfNeuronsCount; j++)
            {
              Neuron *out = outputFiringNeurons[j];
              Dendrite *outDendrite = globalObject->findConnectingDendrite(out, in);
              if (outDendrite == NULL)
              {
                out->connectFrom(in, INHIBITORY);
              }

              Dendrite *inDendrite = globalObject->findConnectingDendrite(in, out);
              if (inDendrite == NULL)
              {
                out->connectTo(in, INHIBITORY); // default to inhibitory
              }
            }
          }
        }

        // As a test, lets fire random association cortex neurons.

        if (randomFireCount < 25)
        {
          TR1Random tr1random;
          int rnd1 = tr1random.generate(1, 100);

          if (rnd1 > 75)
          {

            std::vector<long> associationNeurons = Server::getNeurons("nucleusAssociative", LayerType::input); // Layer input

            for (size_t ix = 0; ix < associationNeurons.size(); ix++)
            {
              int rnd2 = tr1random.generate(1, 100);

              bool selected = (rnd2 > 75);
              if (selected)
              {
                long neuronId = associationNeurons[ix];
                Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
                inputFiringNeurons.push_back(neuron);
              }
            }
            randomFireCount++;
          }
        }

        long delay = PROPAGATION_DELAY_MICROSECONDS;
        long startTimer = globalObject->getCurrentTimestamp();
        long endTimer = startTimer + delay + PROPAGATION_DELAY_MICROSECONDS;

          Range offset = globalObject->batchLearn(&inputFiringNeurons, &outputFiringNeurons, nucleusName);
          long lowestOffset = offset.low;
          long highestOffset = offset.high;
          startTimer = globalObject->getCurrentTimestamp();

          //std::cout << "batchLearn Range: " << offset.low << " - " << offset.high << std::endl;

          delay = PROPAGATION_DELAY_MICROSECONDS;
          if (highestOffset < delay && highestOffset > 0)
            delay = highestOffset;

          if (delay > MAX_TIMEINTERVAL_OFFSET)
            delay = MAX_TIMEINTERVAL_OFFSET;

          endTimer = startTimer + delay + PROPAGATION_DELAY_MICROSECONDS + REFACTORY_PERIOD; 
          // globalObject->cycleNeurons(); // cycle through all neurons a if potiential > threshold, fire the neurons
//          while (globalObject->getAllTotalEvents() > 0)
//          {
//            usleep(250); // 0.25 seconds
//          }
        long ct = globalObject->getCurrentTimestamp();
        while(ct < endTimer)
        {
          usleep(25);
          ct = globalObject->getCurrentTimestamp();
        }
/*        
        int max_loop_count = 60;
        long loopcount = 0;
        long totalEvents = globalObject->getTotalEvents();
        while (totalEvents > 0 && loopcount < max_loop_count) // If more than MAX_ACTIVE_ACTIONPOTENTIALS events in progress - slow it down
        {
          std::cout << "delay current: " << globalObject->getCurrentTimestamp() << ", retry: " << loopcount + 1 << " of " << max_loop_count << " tries." << std::endl;
          sleep(1); // 1 sec delay to allow for ap propagation
          totalEvents = globalObject->getTotalEvents();
          loopcount++;
        }

*/        
        // proceed anyway and hope they die down.... may need some logic to kill
        std::stringstream ss;
        ss << "complete for - " << globalObject->latestOutputTarget << std::endl;
        globalObject->writeFirePatternLog(ss.str().c_str());
      }
      else
      {
        returnCode = 997;
        returnMessage = "nuclei missing: " + command;
        std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        // printf("%s\n",returnString.c_str());
        return returnString;
      }
/*      
      // wait until the input neurons stop firing
      bool done = false;
      size_t infNeuronsCount = inputFiringNeurons.size();
      while(~done)
      {
        done = true;
        for(size_t i=0;i<infNeuronsCount;i++)
        {
          Neuron* n = inputFiringNeurons[i];
          if(n->isFiring())
          {
//            std::cout << "Neuron : " << n->id << " still firing" << std::endl;
            done = false;
          }
        }
        usleep(10);
      }
*/

      returnCode = 0;
      returnMessage = "SUCCESS";
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "GETACTIVATIONPATTERN"))
    {
      //
      // std::string pattern("");
      std::string nucleus = jsonObject["nucleus"];
      if (nucleus.empty())
      {
        returnCode = 500; // invalid object reference
        returnMessage = "invalid object reference";
        std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        return returnString;
      }

      std::vector<long> neurons = Server::getNeurons(nucleus, LayerType::output); // Layer 6 = output
      size_t totalSize = neurons.size();

      size_t tempBufferSize = totalSize / 8 + 1;

      unsigned char *tempBuffer = (unsigned char *)calloc(1, tempBufferSize);

      for (size_t i = 0; i < totalSize; i++)
      {
        long neuronId = neurons[i];
        Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
        int byteindex = i / 8;
        int bitindex = i % 8; // get the modulus

        if (neuron != NULL)
        {
          if (neuron->isFiring(FIRING_WINDOW))
          {
            *(tempBuffer + byteindex) |= bitmapper[bitindex]; // OR the byte to set the particular bit on
          }
        }
      }
      // tempBuffer now contains the activations in binary (on bit indicates neuron is firing)
      // convert the buffer into a base64 string
      std::string encoded = base64_encode(tempBuffer, tempBufferSize);
      // release the tempbuffer
      free(tempBuffer);

      // format the return json string

      returnCode = 0;
      returnMessage = "SUCCESS";

      std::string returnString("{ \"pattern\": \"");
      returnString += encoded;
      returnString += "\", \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "STARTATOMIC"))
    {
      //
      std::string value = jsonObject["value"];

      if (boost::iequals(value, "TRUE"))
      {
      }
      else
      {
      }
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "SETVALUE"))
    {
      //
      /*

      // The variables below can be changed on the fly via the SETVALUE rest interface command
      // example JSON format is { "command": "SETVALUE", "name": "FIRING_WINDOW", "value": 10 }

      // set initial FIRING WINDOW to slightly longer than refactory period
      //#define FIRING_WINDOW 10
      long FIRING_WINDOW = 10;

      //#define PROPAGATION_DELAY_MICROSECONDS 20
      long PROPAGATION_DELAY_MICROSECONDS = 20;

      //#define DECAY_FACTOR  0.01f
      //float DECAY_FACTOR = 0.01f;

      //long REFACTORY_PERIOD = 10;

      // WEIGHT_GRADATION is used to slow the rate of change for weight over time (defined by n updates)
      //#define WEIGHT_GRADATION 10000.0f
      float WEIGHT_GRADATION = 10000.0f;

      // RATE_GRADATION is used to slow the rate of change for rate over time (defined by n updates)
      //#define RATE_GRADATION 10000.0f
      float RATE_GRADATION = 10000.0f;


      */
      //
      std::string key = jsonObject["name"];

      std::string value = jsonObject["value"];

      if (boost::iequals(key, "FIRING_WINDOW"))
      {
        FIRING_WINDOW = std::stol(value);
      }
      else if (boost::iequals(key, "PROPAGATION_DELAY_MICROSECONDS"))
      {
        PROPAGATION_DELAY_MICROSECONDS = std::stol(value);
      }
      else if (boost::iequals(key, "DECAY_FACTOR"))
      {
        DECAY_FACTOR = std::stof(value);
      }
      else if (boost::iequals(key, "REFACTORY_PERIOD"))
      {
        REFACTORY_PERIOD = std::stol(value);
      }
      else if (boost::iequals(key, "WEIGHT_GRADATION"))
      {
        WEIGHT_GRADATION = std::stof(value);
      }
      else if (boost::iequals(key, "RATE_GRADATION"))
      {
        RATE_GRADATION = std::stof(value);
      }
      else
      {
        returnCode = 995;
        returnMessage = "SETVALUE command key or value not valid";
        std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        // printf("%s\n",returnString.c_str());
        return returnString;
      }

      printf("SETVALUE name:%s, value:%s successfully set.\n", key.c_str(), value.c_str());

      returnCode = 0;
      returnMessage = "SUCCESS";
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "GETVALUE"))
    {
      //
      /*

      // The variables below can be retrieved on the fly via the GETVALUE rest interface command
      // example JSON format is { "command": "GETVALUE", "name": "FIRING_WINDOW" }

      */
      //
      std::string key = jsonObject["name"];

      std::string value;

      if (boost::iequals(key, "FIRING_WINDOW"))
      {
        value = std::to_string(FIRING_WINDOW);
      }
      else if (boost::iequals(key, "PROPAGATION_DELAY_MICROSECONDS"))
      {
        value = std::to_string(PROPAGATION_DELAY_MICROSECONDS);
      }
      else if (boost::iequals(key, "DECAY_FACTOR"))
      {
        value = std::to_string(DECAY_FACTOR);
      }
      else if (boost::iequals(key, "REFACTORY_PERIOD"))
      {
        value = std::to_string(REFACTORY_PERIOD);
      }
      else if (boost::iequals(key, "WEIGHT_GRADATION"))
      {
        value = std::to_string(WEIGHT_GRADATION);
      }
      else if (boost::iequals(key, "RATE_GRADATION"))
      {
        value = std::to_string(RATE_GRADATION);
      }
      else
      {
        returnCode = 995;
        returnMessage = "SETVALUE command key or value not valid";
        std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
        // printf("%s\n",returnString.c_str());
        return returnString;
      }

      printf("GETVALUE name:%s, value:%s \n", key.c_str(), value.c_str());

      returnCode = 0;
      returnMessage = "SUCCESS";
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\", \"Value\": \"" + value + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "FLUSH"))
    {
      //
      std::cout << "FLUSH command received" << std::endl;
      globalObject->flush(); // flush all databases - persist all Databases to disc
      std::cout << "FLUSH complete." << std::endl;
      returnCode = 0;
      returnMessage = "SUCCESS";
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "STAT"))
    {
      //
      std::cout << "STAT command received" << std::endl;
      std::string resp = "";
      std::string sep = "";

      std::stringstream s1;
      s1 << "Total Events :" << globalObject->getAllTotalEvents() << std::endl;
      resp = s1.str();

      for (int i = 0; i < FIRING_WINDOW * 2; i++)
      {
        std::stringstream ss;
        long thistimestamp = (globalObject->getCurrentTimestamp() - FIRING_WINDOW) + i; //
        long thisindex = thistimestamp % MAX_TIMEINTERVAL_BUFFER_SIZE;
        long count = globalObject->timeIntervalEvents[thisindex].size();
        ss << sep << (i - FIRING_WINDOW) << ":" << count;
        resp += ss.str();
        sep = ",";
      }

      std::cout << "STAT complete." << std::endl;
      returnCode = 0;
      returnMessage = "SUCCESS";
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\", \"Response\": [" + resp + "] }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else if (boost::iequals(command, "REPORT"))
    {
      returnCode = 998;
      returnMessage = "command not currently implemented: " + command;
      std::string returnString = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";
      // printf("%s\n",returnString.c_str());
      return returnString;
    }
    else
    {
      returnCode = 999;
      returnMessage = "Unknown command received: " + command;

      std::string responseContent = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";

      return responseContent;
    }
  }
  else
  {
    returnCode = 997;
    returnMessage = "empty command received: " + command;

    std::string responseContent = "{ \"Returncode\": \"" + std::to_string(returnCode) + "\", \"ReturnMessage\": \"" + returnMessage + "\" }";

    return responseContent;
  }

  std::string emptyresponseContent;
  return emptyresponseContent;
}

std::string computeResponse(std::string content)
{
  long max_allowed = MAX_ACTIVE_ACTIONPOTENTIALS;
  //  long max_allowed = 100;

  if (globalObject->getTotalEvents() > (long)(max_allowed)) // If more than MAX_ACTIVE_ACTIONPOTENTIALS events in progress - slow it down
  {
    std::cout << "computeResponse: More than " << max_allowed << "  events in progress. Slow it down." << std::endl;
    std::cout << std::endl;

    long currentTotalEvents = globalObject->getTotalEvents();
    long previousTotalEvents = currentTotalEvents;

    bool keepWaiting = true;
    constexpr int MAX_UNCHANGED=100;
    int unchangedCount=0;

    while (currentTotalEvents > (long)(max_allowed) && keepWaiting)
    {
      sleep(5); // sleep for 5 seconds
      previousTotalEvents = currentTotalEvents;
      currentTotalEvents = globalObject->getTotalEvents();
      if (currentTotalEvents < previousTotalEvents)
      {
        std::cout << "computeResponse: Current event count is " << currentTotalEvents << ", down from " << previousTotalEvents << " by " << previousTotalEvents - currentTotalEvents << ".      \r";
        unchangedCount=0;
      }
      else if (currentTotalEvents > previousTotalEvents)
      {
        std::cout << "computeResponse: Current event count is " << currentTotalEvents << ", up from " << previousTotalEvents << " by " << currentTotalEvents - previousTotalEvents << ".        \r";
        unchangedCount=0;
      }
      else
      {
        std::cout << "computeResponse: Current event count is " << currentTotalEvents << ", unchanged from previous count.                                                                      \r";
        unchangedCount++;
        if(unchangedCount>MAX_UNCHANGED)
          keepWaiting = false;
      }
    }
    std::cout << std::endl
              << "computeResponse: Continuing." << std::endl;
  }

  std::string responseContent = parseAndRespondJSON(content);
  return responseContent;
}

std::string computeStatus(std::string content)
{

  std::string responseContent = parseAndRespond(content);
  return responseContent;
}

extern int main_process(Brain *brain)
{

  (void)brain;
  pid_t tid = syscall(SYS_gettid);
//  std::cout << "httpServerMain.main_process thread is " << tid << std::endl;

  std::string host = "0.0.0.0";
  int port = 8124;
  HttpServer server(host, port);

  //

  // Register a few endpoints for demo and benchmarking
  // auto say_hello = [](const HttpRequest &request) -> HttpResponse
  //{
  //  HttpResponse response(HttpStatusCode::Ok);
  //  response.SetHeader("Content-Type", "text/plain");
  //  response.SetContent("Hello, world\n");
  //  return response;
  //};

  auto say_json = [](const HttpRequest &request) -> HttpResponse
  {
    HttpResponse response(HttpStatusCode::Created);
    response.SetHeader("Content-Type", "application/json");
    // Compute response

    pid_t tid = syscall(SYS_gettid);
//    std::cout << "httpServerMain.response thread " << tid << " starting." << std::endl;

    std::string content = computeResponse(request.content());
    response.SetContent(content);

    long totalEvents = globalObject->getTotalEvents();
//    std::cout << "httpServerMain.response thread " << tid << " ending with " << totalEvents << " events remaining. Global Counter: " << globalObject->getTimedEventsCounter() << std::endl;
    return response;
  };

  // auto send_html = [](const HttpRequest &request) -> HttpResponse
  //{
  //   HttpResponse response(HttpStatusCode::Ok);
  //   std::string content;
  //   content += "<!doctype html>\n";
  //   content += "<html>\n<body>\n\n";
  //   content += "<h1>Hello, world in an Html page</h1>\n";
  //   content += "<p>A Paragraph</p>\n\n";
  //   content += "</body>\n</html>\n";
  //
  //   response.SetHeader("Content-Type", "text/html");
  //   response.SetContent(content);
  //   return response;
  // };

  auto send_status = [](const HttpRequest &request) -> HttpResponse
  {
    HttpResponse response(HttpStatusCode::Ok);
    std::string content;
    content += "<!doctype html>\n";
    content += "<html>\n<body>\n\n";
    content += "<h1>SNNEngine Status</h1>\n";
    content += computeStatus(request.content());
    content += "</body>\n</html>\n";

    response.SetHeader("Content-Type", "text/html");
    response.SetContent(content);
    return response;
  };

  //  server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, say_hello);
  //  server.RegisterHttpRequestHandler("/", HttpMethod::GET, say_hello);
  //  server.RegisterHttpRequestHandler("/hello.html", HttpMethod::HEAD, send_html);
  //  server.RegisterHttpRequestHandler("/hello.html", HttpMethod::GET, send_html);
  server.RegisterHttpRequestHandler("/api/data", HttpMethod::POST, say_json);
  server.RegisterHttpRequestHandler("/status", HttpMethod::GET, send_status);
  server.RegisterHttpRequestHandler("/networkStatus.json", HttpMethod::GET, show_network_status);
  server.RegisterHttpRequestHandler("/networkView",       HttpMethod::GET, show_network_view);

  try
  {
    // std::cout << "Setting new limits for file descriptor count.." <<
    // std::endl; ensure_enough_resource(RLIMIT_NOFILE, 15000, 15000);

    // std::cout << "Setting new limits for number of threads.." << std::endl;
    // ensure_enough_resource(RLIMIT_NPROC, 60000, 60000);

    std::cout << "Starting the web server on " << host << ":" << port << std::endl;
    server.Start();
    std::cout << "Server listening on " << host << ":" << port << std::endl;

    // std::cout << "Enter [quit] to stop the server" << std::endl;
    std::string command;
    while (globalObject->keepWebserverRunning)
    { // std::cin >> command, command != "quit") {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "Stopping the web server.." << std::endl;
    server.Stop();
    std::cout << "Server stopped" << std::endl;
  }
  catch (std::exception &e)
  {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}

std::string convertToBinaryString(unsigned char *data, size_t size)
{
  std::string returnString;

  for (int i = 0; i < (int)size; i++)
  {
    int byteindex = i / 8;
    int bitindex = i % 8;

    unsigned char value = data[byteindex] & bitmapper[bitindex];
    if (value == '\0')
    {
      returnString += "0";
    }
    else
    {
      returnString += "1";
    }
  }

  return returnString;
}

std::vector<long> getAllComponentIds(void)
{
  std::vector<long> neurons;
  long neuronIdStart = globalObject->componentBase[ComponentTypeNeuron];
	long neuronIdEnd = globalObject->componentCounter[ComponentTypeNeuron];
  for(long n=neuronIdStart; n<neuronIdEnd;n++)
  {
    neurons.push_back(n);
	}
  return neurons;
}

HttpResponse show_network_status(const HttpRequest &request)
{
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "application/json");

    // We'll build a JSON structure using nlohmann::json
    nlohmann::json networkJson;
    networkJson["neurons"] = nlohmann::json::array();
    networkJson["links"]   = nlohmann::json::array();

    // 1) Gather the neurons
    //    Adjust this to however you retrieve your neuron IDs.
    //    For example: auto neuronIds = globalObject->neuronDB.getAllComponentIds();
    std::vector<long>  neuronIds = getAllComponentIds();

    // We'll map neuronId -> index so we can reference them in "links"
    std::unordered_map<long, size_t> neuronIndexMap;
    size_t index = 0;

    for (auto &nid : neuronIds)
    {
        Neuron* n = globalObject->neuronDB.getComponent(nid);
        if (!n) continue;

        // Build a JSON object for each neuron
        nlohmann::json neuronJson;
        neuronJson["id"]        = (long)n->id;
        neuronJson["threshold"] = n->threshold;
        neuronJson["potential"] = n->getMembranePotential();
        neuronJson["firing"]    = n->isFiring() ? true : false;

        // Optionally include location
        neuronJson["x"] = n->location.x;
        neuronJson["y"] = n->location.y;
        neuronJson["z"] = n->location.z;

        networkJson["neurons"].push_back(neuronJson);

        // Store the index
        neuronIndexMap[n->id] = index++;
    }

    // 2) Gather synapse connections as "links"
    //    For each neuron, we look at its dendrites and retrieve the synapse info
    for (auto &nid : neuronIds)
    {
        Neuron* postNeuron = globalObject->neuronDB.getComponent(nid);
        if (!postNeuron) continue;

        auto& dendrites = *(postNeuron->getDendrites());
        for (auto dId : dendrites)
        {
            Dendrite* d = globalObject->dendriteDB.getComponent(dId);
            if (!d) continue;

            long preId = d->getPreSynapticNeuronId();
            Synapse* s = globalObject->synapseDB.getComponent(d->getSynapseId());
            if (!s) continue;

            // Build a JSON link
            nlohmann::json linkJson;
            linkJson["source"]   = (long)preId;          // actual neuron ID
            linkJson["target"]   = (long)postNeuron->id; // actual neuron ID
            linkJson["weight"]   = s->getWeight();
            linkJson["polarity"] = s->getPolarity();

            // For force-directed or zero-based indexing, you can also store the index:
            linkJson["sourceIdx"] = (long)neuronIndexMap[preId];
            linkJson["targetIdx"] = (long)neuronIndexMap[postNeuron->id];

            networkJson["links"].push_back(linkJson);
        }
    }

    // Convert to string
    std::string jsonString = networkJson.dump();
    response.SetContent(jsonString);

    return response;
};

HttpResponse show_network_view(const HttpRequest &request)

{
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/html");

    // This HTML includes embedded JavaScript (D3.js) which
    // fetches /networkStatus.json and displays a force-directed graph.
    // Feel free to customize styling and behavior.
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "<head>\n"
       << "  <meta charset='utf-8'>\n"
       << "  <title>SNN Network Visualization</title>\n"
       << "  <script src='https://d3js.org/d3.v7.min.js'></script>\n"
       << "  <style>\n"
       << "    .link { stroke: #999; stroke-opacity: 0.6; }\n"
       << "    .node { stroke: #fff; stroke-width: 1.5px; cursor: pointer; }\n"
       << "    .node.firing { fill: red; }\n"
       << "    .node.notfiring { fill: steelblue; }\n"
       << "    text { font-family: sans-serif; font-size: 12px; }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <h1>SNN Network Visualization</h1>\n"
       << "  <div id='chart'></div>\n"
       << "  <script>\n"
       << "    const width = 800, height = 600;\n"
       << "    const svg = d3.select('#chart').append('svg')\n"
       << "        .attr('width', width)\n"
       << "        .attr('height', height);\n"
       << "\n"
       << "    d3.json('/networkStatus.json').then(function(data) {\n"
       << "      const nodes = data.neurons;\n"
       << "      const links = data.links;\n"
       << "\n"
       << "      const simulation = d3.forceSimulation(nodes)\n"
       << "        .force('link', d3.forceLink(links).id(d => d.id))\n"
       << "        .force('charge', d3.forceManyBody().strength(-30))\n"
       << "        .force('center', d3.forceCenter(width / 2, height / 2));\n"
       << "\n"
       << "      const link = svg.selectAll('.link')\n"
       << "        .data(links)\n"
       << "        .enter().append('line')\n"
       << "        .attr('class', 'link')\n"
       << "        .style('stroke-width', d => Math.max(1, d.weight * 2));\n"
       << "\n"
       << "      const node = svg.selectAll('.node')\n"
       << "        .data(nodes)\n"
       << "        .enter().append('circle')\n"
       << "        .attr('class', d => 'node ' + (d.firing ? 'firing' : 'notfiring'))\n"
       << "        .attr('r', 8)\n"
       << "        .call(d3.drag()\n"
       << "          .on('start', dragstarted)\n"
       << "          .on('drag', dragged)\n"
       << "          .on('end', dragended));\n"
       << "\n"
       << "      node.append('title')\n"
       << "        .text(d => `Neuron: ${d.id}\\nThreshold: ${d.threshold}\\nPotential: ${d.potential}`);\n"
       << "\n"
       << "      simulation.on('tick', () => {\n"
       << "        link\n"
       << "          .attr('x1', d => d.source.x)\n"
       << "          .attr('y1', d => d.source.y)\n"
       << "          .attr('x2', d => d.target.x)\n"
       << "          .attr('y2', d => d.target.y);\n"
       << "\n"
       << "        node\n"
       << "          .attr('cx', d => d.x)\n"
       << "          .attr('cy', d => d.y);\n"
       << "      });\n"
       << "\n"
       << "      function dragstarted(event, d) {\n"
       << "        if (!event.active) simulation.alphaTarget(0.3).restart();\n"
       << "        d.fx = d.x;\n"
       << "        d.fy = d.y;\n"
       << "      }\n"
       << "      function dragged(event, d) {\n"
       << "        d.fx = event.x;\n"
       << "        d.fy = event.y;\n"
       << "      }\n"
       << "      function dragended(event, d) {\n"
       << "        if (!event.active) simulation.alphaTarget(0);\n"
       << "        d.fx = null;\n"
       << "        d.fy = null;\n"
       << "      }\n"
       << "    });\n"
       << "  </script>\n"
       << "</body>\n"
       << "</html>\n";

    response.SetContent(ss.str());
    return response;
};
