/*
 * Proprietary License
 * 
 * Copyright (c) 2024 Dean S Horak
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

#include "TR1Random.h"
#include "SpatialDetails.h"


void SpatialDetails::randomizeLocation() {
	// TODO:
	// using the area as the dimensions geneate a random location somewhere within those dimensions
	// convert the dimensions into integers required by the randomizer, by multiplying by 1000, giving us 1/1000 resolution
	location.x = ((float)(tr1random->generate(1, (int)(area.w * 1000))) / 1000.0f);
	location.y = ((float)(tr1random->generate(1, (int)(area.h * 1000))) / 1000.0f);
	location.z = ((float)(tr1random->generate(1, (int)(area.d * 1000))) / 1000.0f);

}