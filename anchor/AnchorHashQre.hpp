// MIT License

// Copyright (c) 2020 anchorhash

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <iostream>
#include <stack>
#include <stdint.h>

/** Class declaration */
class AnchorHashQre {
	
  private:
	
	// Anchor		
	uint32_t *A;

	// Working
	uint32_t *W;

	// Last appearance 
	uint32_t *L;

	// "Map diagonal"	
	uint32_t *K;
	       			
	// Size of the anchor
	uint32_t M;
		
	// Size of the working
	uint32_t N;
	
	// Removed buckets
	std::stack<uint32_t> r;
            
	// Translation oracle
	uint32_t ComputeTranslation(uint32_t i , uint32_t j);
					
  public:
  
	AnchorHashQre (uint32_t, uint32_t);
	
	~AnchorHashQre();
		
	uint32_t ComputeBucket(uint64_t, uint64_t);
        
	uint32_t UpdateRemoval(uint32_t);
    
	uint32_t UpdateNewBucket();
           
};
