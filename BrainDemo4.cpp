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

#include "Global.h"
#include "TR1Random.h"
#include "BrainDemo4.h"

BrainDemo4::BrainDemo4(void)
{
}

BrainDemo4::~BrainDemo4(void)
{
}

Brain * BrainDemo4::create(bool rebuild) 
{

	Brain *brain; 

	std::stringstream ss;

	SpatialDetails sd(1, 2, 3, 4, 5, 6); // Dummy test locations/size


	if(!rebuild) // if not rebuilding, just return brain
	{
		LOGSTREAM(ss) << " Loading brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::load();
	} 
	else 
	{
		LOGSTREAM(ss) << " Creating brain" << std::endl;
		globalObject->log(ss);
		brain = Brain::create();
	}

	// Create Thalamus
	LOGSTREAM(ss) << "Create Thalamus ... " << std::endl;
	globalObject->log(ss);

	Region *regionThalamus = 0L;
	if(brain->restartpoint())
	{
		regionThalamus = Region::create("regionThalamus", sd);
		brain->add(regionThalamus);
	} 
	else 
	{
		globalObject->regionDB.next();
		regionThalamus = globalObject->regionDB.getValue();
		LOGSTREAM(ss) << "regionThalamus globalObject->regionDB.getValue() " << std::endl;
		globalObject->log(ss);
	}
	brain->syncpoint();


	// Create Thalamic Nuclei
	ColumnNeuronProfile profile; // default profile for all layers is Pyramidal neurons, 100 neurons per cluster, with 10 clusters

	Nucleus *nucleusAnteroventral = 0L;
	if(brain->restartpoint())
	{
		nucleusAnteroventral = Nucleus::create("nucleusAnteroventral", sd);
		regionThalamus->add(nucleusAnteroventral);
		nucleusAnteroventral->addColumns(10,6,5,10); // 10 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
	} 
	else 
	{
		globalObject->nucleusDB.next();
		nucleusAnteroventral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusAnterodorsal = 0L;
	if(brain->restartpoint())
	{
		nucleusAnterodorsal = Nucleus::create("nucleusAnterodorsal", sd);
		regionThalamus->add(nucleusAnterodorsal);
		nucleusAnterodorsal->addColumns(10,6,5,10); 
	} 
	else 
	{
		globalObject->nucleusDB.next();
		nucleusAnterodorsal = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusAnteromedial = 0L;
	if(brain->restartpoint())
	{
		nucleusAnteromedial = Nucleus::create("nucleusAnteromedial", sd);
		regionThalamus->add(nucleusAnteromedial);
		nucleusAnteromedial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusAnteromedial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusSuperficial = 0L;
	if(brain->restartpoint())
	{
		nucleusSuperficial = Nucleus::create("nucleusSuperficial", sd);
		regionThalamus->add(nucleusSuperficial);
		nucleusSuperficial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusSuperficial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusParvocellular = 0L;
	if(brain->restartpoint())
	{
		nucleusParvocellular = Nucleus::create("nucleusParvocellular", sd);
		regionThalamus->add(nucleusParvocellular);
		nucleusParvocellular->addColumns(10,6,5,10); 
	} 
	else
	{
		globalObject->nucleusDB.next();
		nucleusParvocellular = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusMagnocellular = 0L;
	if(brain->restartpoint())
	{
		nucleusMagnocellular = Nucleus::create("nucleusMagnocellular", sd);
		regionThalamus->add(nucleusMagnocellular);
		nucleusMagnocellular->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusMagnocellular = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusParatenial = 0L;
	if(brain->restartpoint())
	{
		nucleusParatenial = Nucleus::create("nucleusParatenial", sd);
		regionThalamus->add(nucleusParatenial);
		nucleusParatenial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusParatenial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusParventricular = 0L;
	if(brain->restartpoint())
	{
		nucleusParventricular = Nucleus::create("nucleusParventricular", sd);
		regionThalamus->add(nucleusParventricular);
		nucleusParventricular->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusParventricular = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusReuniens = 0L;
	if(brain->restartpoint())
	{
		nucleusReuniens = Nucleus::create("nucleusReuniens", sd);
		regionThalamus->add(nucleusReuniens);
		nucleusReuniens->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusReuniens = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusRhomboidal = 0L;
	if(brain->restartpoint())
	{
		nucleusRhomboidal = Nucleus::create("nucleusRhomboidal", sd);
		regionThalamus->add(nucleusRhomboidal);
		nucleusRhomboidal->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusRhomboidal = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusParacentral = 0L;
	if(brain->restartpoint())
	{
		nucleusParacentral = Nucleus::create("nucleusParacentral", sd);
		regionThalamus->add(nucleusParacentral);
		nucleusParacentral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusParacentral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusCentrallateral = 0L;
	if(brain->restartpoint())
	{
		nucleusCentrallateral = Nucleus::create("nucleusCentrallateral", sd);
		regionThalamus->add(nucleusCentrallateral);
		nucleusCentrallateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusCentrallateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusCentralmedial = 0L;
	if(brain->restartpoint())
	{
		nucleusCentralmedial = Nucleus::create("nucleusCentralmedial", sd);
		regionThalamus->add(nucleusCentralmedial);
		nucleusCentralmedial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusCentralmedial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusCentromedian = 0L;
	if(brain->restartpoint())
	{
		nucleusCentromedian = Nucleus::create("nucleusCentromedian", sd);
		regionThalamus->add(nucleusCentromedian);
		nucleusCentromedian->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusCentromedian = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusParafascicular = 0L;
	if(brain->restartpoint())
	{
		nucleusParafascicular = Nucleus::create("nucleusParafascicular", sd);
		regionThalamus->add(nucleusParafascicular);
		nucleusParafascicular->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusParafascicular = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPulvinar = 0L;
	if(brain->restartpoint())
	{
		nucleusPulvinar = Nucleus::create("nucleusPulvinar", sd);
		regionThalamus->add(nucleusPulvinar);
		nucleusPulvinar->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPulvinar = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusLateralposterior = 0L;
	if(brain->restartpoint())
	{
		nucleusLateralposterior = Nucleus::create("nucleusLateralposterior", sd);
		regionThalamus->add(nucleusLateralposterior);
		nucleusLateralposterior->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusLateralposterior = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusLateraldorsal = 0L;
	if(brain->restartpoint())
	{
		nucleusLateraldorsal = Nucleus::create("nucleusLateraldorsal", sd);
		regionThalamus->add(nucleusLateraldorsal);
		nucleusLateraldorsal->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusLateraldorsal = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVentralanterior = 0L;
	if(brain->restartpoint())
	{
		nucleusVentralanterior = Nucleus::create("nucleusVentralanterior", sd);
		regionThalamus->add(nucleusVentralanterior);
		nucleusVentralanterior->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVentralanterior = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVentrallateral = 0L;
	if(brain->restartpoint())
	{
		nucleusVentrallateral = Nucleus::create("nucleusVentrallateral", sd);
		regionThalamus->add(nucleusVentrallateral);
		nucleusVentrallateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVentrallateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVentralposterior = 0L;
	if(brain->restartpoint())
	{
		nucleusVentralposterior = Nucleus::create("nucleusVentralposterior", sd);
		regionThalamus->add(nucleusVentralposterior);
		nucleusVentralposterior->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVentralposterior = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVentralposterolateral = 0L;
	if(brain->restartpoint())
	{
		nucleusVentralposterolateral = Nucleus::create("nucleusVentralposterolateral", sd);
		regionThalamus->add(nucleusVentralposterolateral);
		nucleusVentralposterolateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVentralposterolateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVentralposteromedial = 0L;
	if(brain->restartpoint())
	{
		nucleusVentralposteromedial = Nucleus::create("nucleusVentralposteromedial", sd);
		regionThalamus->add(nucleusVentralposteromedial);
		nucleusVentralposteromedial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVentralposteromedial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusMedialgeniculate = 0L;
	if(brain->restartpoint())
	{
		nucleusMedialgeniculate = Nucleus::create("nucleusMedialgeniculate", sd);
		regionThalamus->add(nucleusMedialgeniculate);
		nucleusMedialgeniculate->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusMedialgeniculate = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusLateralgeniculate = 0L;
	if(brain->restartpoint())
	{
		nucleusLateralgeniculate = Nucleus::create("nucleusLateralgeniculate", sd);
		regionThalamus->add(nucleusLateralgeniculate);
		nucleusLateralgeniculate->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusLateralgeniculate = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusThalamicreticular = 0L;
	if(brain->restartpoint())
	{
		nucleusThalamicreticular = Nucleus::create("nucleusThalamicreticular", sd);
		regionThalamus->add(nucleusThalamicreticular);
		nucleusThalamicreticular->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusThalamicreticular = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();


	LOGSTREAM(ss) << "Region " << regionThalamus->name << " complete with " << regionThalamus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create Basal Ganglia
	LOGSTREAM(ss) << "Create Basal Ganglia ... " << std::endl;
	globalObject->log(ss);

	Region *regionBasalGanglia = 0L;
	if(brain->restartpoint())
	{
		regionBasalGanglia = Region::create("regionBasalGanglia", sd);
		brain->add(regionBasalGanglia);
	}
	else
	{
		globalObject->regionDB.next();
		regionBasalGanglia = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusStriatum = 0L;
	if(brain->restartpoint())
	{
		nucleusStriatum = Nucleus::create("nucleusStriatum", sd);
		regionBasalGanglia->add(nucleusStriatum);
		nucleusStriatum->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusStriatum = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPallidum = 0L;
	if(brain->restartpoint())
	{
		nucleusPallidum = Nucleus::create("nucleusPallidum", sd);
		regionBasalGanglia->add(nucleusPallidum);
		nucleusPallidum->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPallidum = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusSubstantiaNigra = 0L;
	if(brain->restartpoint())
	{
		nucleusSubstantiaNigra = Nucleus::create("nucleusSubstantiaNigra", sd);
		regionBasalGanglia->add(nucleusSubstantiaNigra);
		nucleusSubstantiaNigra->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusSubstantiaNigra = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusSubthalamaic = 0L;
	if(brain->restartpoint())
	{
		nucleusSubthalamaic = Nucleus::create("nucleusSubthalamaic", sd);
		regionBasalGanglia->add(nucleusSubthalamaic);
		nucleusSubthalamaic->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusSubthalamaic = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionBasalGanglia->name << " complete with " << regionBasalGanglia->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create Sensory Cortex
	LOGSTREAM(ss) << "Create Sensory Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionSensoryCortex = 0L;
	if(brain->restartpoint())
	{
		regionSensoryCortex = Region::create("regionSensoryCortex", sd);
		brain->add(regionSensoryCortex);
	}
	else
	{
		globalObject->regionDB.next();
		regionSensoryCortex = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	// Sensory modalities - one large nucleus per modality
	Nucleus *nucleusAuditory = 0L;
	if(brain->restartpoint())
	{
		nucleusAuditory = Nucleus::create("nucleusAuditory", sd);
		regionSensoryCortex->add(nucleusAuditory);
		nucleusAuditory->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusAuditory = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusWernicke = 0L;
	if(brain->restartpoint())
	{
		nucleusWernicke = Nucleus::create("nucleusWernicke", sd); // language intepretation
		regionSensoryCortex->add(nucleusWernicke);
		nucleusWernicke->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusWernicke = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVisual = 0L;
	if(brain->restartpoint())
	{
		nucleusVisual = Nucleus::create("nucleusVisual", sd);
		regionSensoryCortex->add(nucleusVisual);
		nucleusVisual->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVisual = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionSensoryCortex->name << " complete with " << regionSensoryCortex->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create Motor Cortex
	LOGSTREAM(ss) << "Create Motor Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionMotorCortex = 0L;
	if(brain->restartpoint())
	{
		regionMotorCortex = Region::create("regionMotorCortex", sd);
		brain->add(regionMotorCortex);
	}
	else
	{
		globalObject->regionDB.next();
		regionMotorCortex = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	// Motor modalities - one large nucleus per modality
	Nucleus *nucleusPeriaqueductalGray = 0L;  // vocalization control
	if(brain->restartpoint())
	{
		nucleusPeriaqueductalGray = Nucleus::create("nucleusPeriaqueductalGray ", sd);
		regionMotorCortex->add(nucleusPeriaqueductalGray);
		nucleusPeriaqueductalGray->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPeriaqueductalGray = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusBroca = 0L;
	if(brain->restartpoint())
	{
		nucleusBroca = Nucleus::create("nucleusBroca", sd); // language production
		regionMotorCortex->add(nucleusBroca);
		nucleusBroca->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusBroca = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusOculoMotor = 0L;
	if(brain->restartpoint())
	{
		nucleusOculoMotor = Nucleus::create("nucleusOculoMotor", sd); // visual output production
		regionMotorCortex->add(nucleusOculoMotor);
		nucleusOculoMotor->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusOculoMotor = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionMotorCortex->name << " complete with " << regionMotorCortex->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Create PreFrontal
	LOGSTREAM(ss) << "Create PreFrontal Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionPrefrontalCortex = 0L;
	if(brain->restartpoint())
	{
		regionPrefrontalCortex = Region::create("regionPrefrontalCortex", sd);
		brain->add(regionPrefrontalCortex);
	}
	else
	{
		globalObject->regionDB.next();
		regionPrefrontalCortex = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCPosteriorDorsolateral = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCPosteriorDorsolateral = Nucleus::create("nucleusPFCPosteriorDorsolateral", sd);
		regionPrefrontalCortex->add(nucleusPFCPosteriorDorsolateral);
		nucleusPFCPosteriorDorsolateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCPosteriorDorsolateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCMedialDorsolateral = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCMedialDorsolateral = Nucleus::create("nucleusPFCMedialDorsolateral", sd);
		regionPrefrontalCortex->add(nucleusPFCMedialDorsolateral);
		nucleusPFCMedialDorsolateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCMedialDorsolateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCVentrolateral = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCVentrolateral = Nucleus::create("nucleusPFCVentrolateral", sd);
		regionPrefrontalCortex->add(nucleusPFCVentrolateral);
		nucleusPFCVentrolateral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCVentrolateral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCOrbitofrontal = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCOrbitofrontal = Nucleus::create("nucleusPFCOrbitofrontal", sd);
		regionPrefrontalCortex->add(nucleusPFCOrbitofrontal);
		nucleusPFCOrbitofrontal->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCOrbitofrontal = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCVentromedial = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCVentromedial = Nucleus::create("nucleusPFCVentromedial", sd);
		regionPrefrontalCortex->add(nucleusPFCVentromedial);
		nucleusPFCVentromedial->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCVentromedial = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCBasal = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCBasal = Nucleus::create("nucleusPFCBasal", sd);
		regionPrefrontalCortex->add(nucleusPFCBasal);
		nucleusPFCBasal->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCBasal = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCOrbital = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCOrbital = Nucleus::create("nucleusPFCOrbital", sd);
		regionPrefrontalCortex->add(nucleusPFCOrbital);
		nucleusPFCOrbital->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCOrbital = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCFrontopolar = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCFrontopolar = Nucleus::create("nucleusPFCFrontopolar", sd);
		regionPrefrontalCortex->add(nucleusPFCFrontopolar);
		nucleusPFCFrontopolar->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCFrontopolar = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCAnterior = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCAnterior = Nucleus::create("nucleusPFCAnterior", sd);
		regionPrefrontalCortex->add(nucleusPFCAnterior);
		nucleusPFCAnterior->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCAnterior = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPFCRostral = 0L;
	if(brain->restartpoint())
	{
		nucleusPFCRostral = Nucleus::create("nucleusPFCRostral", sd);
		regionPrefrontalCortex->add(nucleusPFCRostral);
		nucleusPFCRostral->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPFCRostral = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionPrefrontalCortex->name << " complete with " << regionPrefrontalCortex->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create Hippocampus
	LOGSTREAM(ss) << "Create Hippocampus ... " << std::endl;
	globalObject->log(ss);

	Region *regionHippocampus = 0L;
	if(brain->restartpoint())
	{
		regionHippocampus = Region::create("regionHippocampus", sd);
		brain->add(regionHippocampus);
	}
	else
	{
		globalObject->regionDB.next();
		regionHippocampus = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusHippocampusproper = 0L;
	if(brain->restartpoint())
	{
		nucleusHippocampusproper = Nucleus::create("nucleusHippocampusproper", sd);
		regionHippocampus->add(nucleusHippocampusproper);
		nucleusHippocampusproper->addColumns(10,6,5,10); // 1000 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusHippocampusproper = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusDendategyrus = 0L;
	if(brain->restartpoint())
	{
		nucleusDendategyrus = Nucleus::create("nucleusDendategyrus", sd);
		regionHippocampus->add(nucleusDendategyrus);
		nucleusDendategyrus->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusDendategyrus = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionHippocampus->name << " complete with " << regionHippocampus->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Modality specific regions

	// Create Auditory Cortex
	LOGSTREAM(ss) << "Create Auditory Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionAuditoryCortex = 0L;
	if(brain->restartpoint())
	{
		regionAuditoryCortex = Region::create("regionAuditoryCortex", sd);
		brain->add(regionAuditoryCortex);
	}
	else
	{
		globalObject->regionDB.next();
		regionAuditoryCortex = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusPrimaryauditorycortex = 0L;
	if(brain->restartpoint())
	{
		nucleusPrimaryauditorycortex = Nucleus::create("nucleusPrimaryauditorycortex", sd);
		regionAuditoryCortex->add(nucleusPrimaryauditorycortex);
		nucleusPrimaryauditorycortex->addColumns(10,6,5,10); // 100 columns, each with 6 layers, each with 5 clusters, each with 10 neurons
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusPrimaryauditorycortex = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusCochlear = 0L;
	if(brain->restartpoint())
	{
		nucleusCochlear = Nucleus::create("nucleusCochlear", sd);
		regionAuditoryCortex->add(nucleusCochlear);
		nucleusCochlear->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusCochlear = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusTrapezoid = 0L;
	if(brain->restartpoint())
	{
		nucleusTrapezoid = Nucleus::create("nucleusTrapezoid", sd);
		regionAuditoryCortex->add(nucleusTrapezoid);
		nucleusTrapezoid->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusTrapezoid = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusSuperiorolivarycomplex = 0L;
	if(brain->restartpoint())
	{
		nucleusSuperiorolivarycomplex = Nucleus::create("nucleusSuperiorolivarycomplex", sd);
		regionAuditoryCortex->add(nucleusSuperiorolivarycomplex);
		nucleusSuperiorolivarycomplex->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusSuperiorolivarycomplex = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusLaterallemniscus = 0L;
	if(brain->restartpoint())
	{
		nucleusLaterallemniscus = Nucleus::create("nucleusLaterallemniscus", sd);
		regionAuditoryCortex->add(nucleusLaterallemniscus);
		nucleusLaterallemniscus->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusLaterallemniscus = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusInferiorcolliculi = 0L;
	if(brain->restartpoint())
	{
		nucleusInferiorcolliculi = Nucleus::create("nucleusInferiorcolliculi", sd);
		regionAuditoryCortex->add(nucleusInferiorcolliculi);
		nucleusInferiorcolliculi->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusInferiorcolliculi = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusACMedialgeniculate = 0L;
	if(brain->restartpoint())
	{
		nucleusACMedialgeniculate = Nucleus::create("nucleusACMedialgeniculate", sd);
		regionAuditoryCortex->add(nucleusACMedialgeniculate);
		nucleusACMedialgeniculate->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusACMedialgeniculate = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionAuditoryCortex->name << " complete with " << regionAuditoryCortex->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create Primary Visual Cortex
	LOGSTREAM(ss) << "Create Primary Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionPrimaryVisualCortex = 0L;
	if(brain->restartpoint())
	{
		regionPrimaryVisualCortex = Region::create("regionPrimaryVisualCortex", sd);
		brain->add(regionPrimaryVisualCortex);
	}
	else
	{
		globalObject->regionDB.next();
		regionPrimaryVisualCortex = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusVCLateralgeniculate = 0L;
	if(brain->restartpoint())
	{
		nucleusVCLateralgeniculate = Nucleus::create("nucleusVCLateralgeniculate", sd);
		regionPrimaryVisualCortex->add(nucleusVCLateralgeniculate);
		nucleusVCLateralgeniculate->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusVCLateralgeniculate = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionPrimaryVisualCortex->name << " complete with " << regionPrimaryVisualCortex->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Create V1 Visual Cortex
	LOGSTREAM(ss) << "Create V1 Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionVisualCortexV1 = 0L;
	if(brain->restartpoint())
	{
		regionVisualCortexV1 = Region::create("regionVisualCortexV1", sd);
		brain->add(regionVisualCortexV1);
	}
	else
	{
		globalObject->regionDB.next();
		regionVisualCortexV1 = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusV1= 0L;
	if(brain->restartpoint())
	{
		nucleusV1= Nucleus::create("nucleusV1", sd);
		regionVisualCortexV1->add(nucleusV1);
		nucleusV1->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusV1 = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionVisualCortexV1->name << " complete with " << regionVisualCortexV1->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);

	// Create V2 Visual Cortex
	LOGSTREAM(ss) << "Create V2 Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionVisualCortexV2 = 0L;
	if(brain->restartpoint())
	{
		regionVisualCortexV2 = Region::create("regionVisualCortexV2", sd);
		brain->add(regionVisualCortexV2);
	}
	else
	{
		globalObject->regionDB.next();
		regionVisualCortexV2 = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusV2= 0L;
	if(brain->restartpoint())
	{
		nucleusV2= Nucleus::create("nucleusV2", sd);
		regionVisualCortexV2->add(nucleusV2);
		nucleusV2->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusV2 = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionVisualCortexV2->name << " complete with " << regionVisualCortexV2->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Create V3 Visual Cortex
	LOGSTREAM(ss) << "Create V3 Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionVisualCortexV3 = 0L;
	if(brain->restartpoint())
	{
		regionVisualCortexV3 = Region::create("regionVisualCortexV3", sd);
		brain->add(regionVisualCortexV3);
	}
	else
	{
		globalObject->regionDB.next();
		regionVisualCortexV3 = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusV3= 0L;
	if(brain->restartpoint())
	{
		nucleusV3= Nucleus::create("nucleusV3", sd);
		regionVisualCortexV3->add(nucleusV3);
		nucleusV3->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusV3 = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionVisualCortexV3->name << " complete with " << regionVisualCortexV3->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Create V4 Visual Cortex
	LOGSTREAM(ss) << "Create V4 Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionVisualCortexV4 = 0L;
	if(brain->restartpoint())
	{
		regionVisualCortexV4 = Region::create("regionVisualCortexV4", sd);
		brain->add(regionVisualCortexV4);
	}
	else
	{
		globalObject->regionDB.next();
		regionVisualCortexV4 = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusV4= 0L;
	if(brain->restartpoint())
	{
		nucleusV4= Nucleus::create("nucleusV4", sd);
		regionVisualCortexV4->add(nucleusV4);
		nucleusV4->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusV4 = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionVisualCortexV4->name << " complete with " << regionVisualCortexV4->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Create MT Visual Cortex
	LOGSTREAM(ss) << "Create MT Visual Cortex ... " << std::endl;
	globalObject->log(ss);

	Region *regionVisualCortexMT = 0L;
	if(brain->restartpoint())
	{
		regionVisualCortexMT = Region::create("regionVisualCortexMT", sd);
		brain->add(regionVisualCortexMT);
	}
	else
	{
		globalObject->regionDB.next();
		regionVisualCortexMT = globalObject->regionDB.getValue();
	}
	brain->syncpoint();

	Nucleus *nucleusMT= 0L;
	if(brain->restartpoint())
	{
		nucleusMT= Nucleus::create("nucleusMT", sd);
		regionVisualCortexMT->add(nucleusMT);
		nucleusMT->addColumns(10,6,5,10); 
	}
	else
	{
		globalObject->nucleusDB.next();
		nucleusMT = globalObject->nucleusDB.getValue();
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "Region " << regionVisualCortexMT->name << " complete with " << regionVisualCortexMT->nuclei.size() << " nuclei." << std::endl;
	globalObject->log(ss);


	// Wrap up construction

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attaching layers within columns " << std::endl;
	globalObject->log(ss);
	// Now, attach layers within a column
//	CollectionIterator<Column *> itColumn(Global::getColumnsCollection());

	size_t columnCount = globalObject->columnsSize();
	size_t columnNum = 0;

	for (globalObject->columnDB.begin(); globalObject->columnDB.more(); globalObject->columnDB.next())
	{
		Column *column = globalObject->columnDB.getValue();

		size_t pct = (columnNum*100) / columnCount;
//		std::cout << " Initializing layers within column " << column->id << std::endl;
		LOGSTREAM(ss) << " Initializing layers within column " << column->id << " - (" << columnNum++ << " of " << columnCount << " - " << pct << "%) " << std::endl;
		globalObject->log(ss);

		if(brain->restartpoint())
		{
			//column->initializeLayers(0);
		}
		brain->syncpoint();
	}

	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << globalObject->regionsSize() << " regions created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->nucleiSize() << " nuclei created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->columnsSize() << " columns created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->layersSize() << " layers created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->clustersSize() << " clusters created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->neuronsSize() << " neurons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->axonsSize() << " axons created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << std::endl << "... " << std::endl << std::endl;
	globalObject->log(ss);

	LOGSTREAM(ss) << " Attach regions" << std::endl;
	globalObject->log(ss);
// Finally, attach the Regions


	// Thamamus project to all other regions
	LOGSTREAM(ss) << "regionThalamus->projectTo(regionBasalGanglia)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionBasalGanglia,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionSensoryCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionSensoryCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionMotorCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionMotorCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionPrefrontalCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionPrefrontalCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionHippocampus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionHippocampus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionAuditoryCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionAuditoryCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionPrimaryVisualCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionPrimaryVisualCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionVisualCortexV1)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionVisualCortexV1,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionVisualCortexV2)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionVisualCortexV2,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionVisualCortexV3)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionVisualCortexV3,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionVisualCortexV4)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionVisualCortexV4,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionThalamus->projectTo(regionVisualCortexMT)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionThalamus->projectTo(regionVisualCortexMT,10.f);
	}
	brain->syncpoint();

	// all other regions project to thamamus
	LOGSTREAM(ss) << "regionBasalGanglia->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionBasalGanglia->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionSensoryCortex->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionSensoryCortex->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionMotorCortex->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionMotorCortex->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionPrefrontalCortex->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionPrefrontalCortex->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionHippocampus->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionHippocampus->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionAuditoryCortex->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionAuditoryCortex->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionPrimaryVisualCortex->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionPrimaryVisualCortex->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV1->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV1->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV2->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV2->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV3->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV3->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV4->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV4->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexMT->projectTo(regionThalamus)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexMT->projectTo(regionThalamus,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionPrefrontalCortex->projectTo(regionBasalGanglia)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionPrefrontalCortex->projectTo(regionBasalGanglia,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionBasalGanglia->projectTo(regionMotorCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionBasalGanglia->projectTo(regionMotorCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionSensoryCortex->projectTo(regionPrefrontalCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionSensoryCortex->projectTo(regionPrefrontalCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionAuditoryCortex->projectTo(regionSensoryCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionAuditoryCortex->projectTo(regionSensoryCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexMT->projectTo(regionSensoryCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexMT->projectTo(regionSensoryCortex,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionPrimaryVisualCortex->projectTo(regionVisualCortexV1)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionPrimaryVisualCortex->projectTo(regionVisualCortexV1,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV1->projectTo(regionVisualCortexV2)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV1->projectTo(regionVisualCortexV2,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV2->projectTo(regionVisualCortexV3)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV2->projectTo(regionVisualCortexV3,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV3->projectTo(regionVisualCortexV4)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV3->projectTo(regionVisualCortexV4,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexV4->projectTo(regionVisualCortexMT)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexV4->projectTo(regionVisualCortexMT,10.f);
	}
	brain->syncpoint();

	LOGSTREAM(ss) << "regionVisualCortexMT->projectTo(regionPrefrontalCortex)" << std::endl;
	globalObject->log(ss);
	if(brain->restartpoint())
	{
		regionVisualCortexMT->projectTo(regionPrefrontalCortex,10.f);
	}
	brain->syncpoint();



	LOGSTREAM(ss) << "------------------------------------------------------" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->dendritesSize() << " dendrites created" << std::endl;
	globalObject->log(ss);
	LOGSTREAM(ss) << globalObject->synapsesSize() << " synapses created" << std::endl;
	globalObject->log(ss);

	unsigned long zeroCount = 0;
//	CollectionIterator<Axon *> itAxon(Global::getAxonsCollection());
//	for (itAxon.begin(); itAxon.more(); itAxon.next())
	for (globalObject->axonDB.begin(); globalObject->axonDB.more(); globalObject->axonDB.next())
	{
		if(globalObject->axonDB.getValue()->getSynapses()->size()==0) 
			zeroCount++;
	}
	LOGSTREAM(ss) << zeroCount << " axons have no synapses." << std::endl;
	globalObject->log(ss);

	globalObject->flush();
	return brain;
}

void BrainDemo4::step(Brain *brain)
{
		(void)brain;

//	std::cout << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;


	std::stringstream ss;
//	LOGSTREAM(ss) << "Current timestamp " << globalObject->current_timestep << " Current AP count " << globalObject->actionPotentialsSize() << std::endl;
//	globalObject->log(ss);

	if(globalObject->current_timestep == 10) 
	{
		// At timestep 10, initialize all of the synapses attached to the first axon with a weight of 1.0 
//		CollectionIterator<Neuron *> itNeuron(Global::getNeuronsCollection());
//		for (itNeuron.begin(); itNeuron.more(); itNeuron.next())
		for (globalObject->neuronDB.begin(); globalObject->neuronDB.more(); globalObject->neuronDB.next())
		{
			std::vector<long> *it2 = globalObject->neuronDB.getValue()->getAxons();
			size_t ssize = it2->size();
			for(size_t i=0;i<ssize;i++)
			{
				Axon *axon = globalObject->axonDB.getComponent((*it2)[i]);
				std::vector<long> *it3 = axon->getSynapses();
				size_t ssize = it3->size();
				for(size_t i=0;i<ssize;i++)
				{
//				for (it3.begin(); it3.more(); it3.next())
//				{
					size_t rndA = (size_t) tr1random->generate(1,10); // Random # of Axons
					if(rndA==5) {
						Synapse *s = globalObject->synapseDB.getComponent((*it3)[i]);
						s->setWeight(1.0);
					}
				}
			}
		}
		globalObject->neuronDB.begin(); // Reset iterator to beginning

		// Then fire the first neuron
		Neuron *n = globalObject->neuronDB.getValue();
		n->fire(); // First 1st neuron
	
		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // 21st-41st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
			globalObject->neuronDB.next();
		}


		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // First 61st-81st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
			globalObject->neuronDB.next();
		}
		for(size_t i=0;i<20;i++) { globalObject->neuronDB.next(); }
		for(size_t i=0;i<20;i++) // First 101st-121st neuron
		{
			Neuron *n2 = globalObject->neuronDB.getValue();
			n2->fire(); // First 1st neuron
			globalObject->neuronDB.next();
		}

	}

}
