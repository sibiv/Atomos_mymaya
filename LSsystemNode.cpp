
#include <maya/MFnDependencyNode.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

//#include <maya/MPxNode.h>
//#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>


#include <maya/MIOStream.h>

#include "LSystemNode.h"
#include "LSystem.h"
#include "cylinder.h"

MStatus returnStatus;

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
	cerr << msg;				\
	return MS::kFailure;		\
	}

MObject LSystemNode::time;
MObject LSystemNode::outputMesh;
MObject LSystemNode::angle;
MObject LSystemNode::steps;
MObject LSystemNode::grammar;
MTypeId LSystemNode::id( 0x80000 );

void* LSystemNode::creator()
{
	return new LSystemNode;
}

MStatus LSystemNode::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;

	MFnNumericAttribute angleNumAttr;
	MFnNumericAttribute stepNumAttr;

	MFnTypedAttribute grammarTypedAttr;

	MStatus returnStatus;

	LSystemNode::time = unitAttr.create( "time", "tm",
		MFnUnitAttribute::kTime,
		0.0, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode time attribute\n");


	LSystemNode::outputMesh = typedAttr.create( "outputMesh", "out",
		MFnData::kMesh,
		&returnStatus ); 
	McheckErr(returnStatus, "ERROR creating LSystemNode output attribute\n");
	typedAttr.setStorable(false);

	LSystemNode::angle = angleNumAttr.create( "angle", "a",
		MFnNumericData::kDouble, 90.0 );
	McheckErr(returnStatus, "ERROR creating LSystemNode angle attribute\n");

	LSystemNode::steps = stepNumAttr.create( "steps", "s",
		MFnNumericData::kDouble, 1.0 );
	McheckErr(returnStatus, "ERROR creating LSystemNode step size attribute\n");

	LSystemNode::grammar = grammarTypedAttr.create( "grammar", "g",
		MFnData::kString, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode grammar attribute\n");
	grammarTypedAttr.setStorable(false);


	returnStatus = addAttribute(LSystemNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = addAttribute(LSystemNode::angle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");

	returnStatus = addAttribute(LSystemNode::steps);
	McheckErr(returnStatus, "ERROR adding steps attribute\n");

	returnStatus = addAttribute(LSystemNode::grammar);
	McheckErr(returnStatus, "ERROR adding grammar attribute\n");



	returnStatus = attributeAffects(LSystemNode::time,
		LSystemNode::outputMesh);
	returnStatus = attributeAffects(LSystemNode::angle,
		LSystemNode::outputMesh);
	returnStatus = attributeAffects(LSystemNode::grammar,
		LSystemNode::outputMesh);
	returnStatus = attributeAffects(LSystemNode::steps,
		LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}


MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus returnStatus;

	if (plug == outputMesh) {
		/* Get time */
		MDataHandle timeData = data.inputValue( time, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		MTime time = timeData.asTime();

		MDataHandle angleData = data.inputValue( angle, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		double angle_value = angleData.asDouble();

		MDataHandle stepsData = data.inputValue( steps, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		double steps_value = stepsData.asDouble();

		MDataHandle grammarData = data.inputValue( grammar, &returnStatus ); 
		McheckErr(returnStatus, "Error getting time data handle\n");
		MString grammar_value = grammarData.asString();

		/* Get output object */

		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		MFnMesh	myMesh;
		MPointArray points;
		MIntArray faceCounts;
		MIntArray faceConnects;

		//MString grammar = ("F\\nF->F[+F]F[-F]F");

		CylinderMesh *cm;


		LSystem system;
		system.loadProgramFromString(grammar_value.asChar());
		system.setDefaultAngle(angle_value);
		system.setDefaultStep(steps_value);



			std::vector<LSystem::Branch> branches;
			system.process(time.value(), branches);

			int k = branches.size();
			for(int j = 0; j < branches.size(); j++)
			{
				//1. find the position for start and end point of current branch
				//2. generate a cylinder
				MPoint start(branches[j].first[0],branches[j].first[1],branches[j].first[2]);
				MPoint end(branches[j].second[0],branches[j].second[1],branches[j].second[2]);
				cm = new CylinderMesh(start, end);
				cm->appendToMesh(points, faceCounts, faceConnects); 
			}

		MObject newMesh = myMesh.create(points.length(), faceCounts.length(),
			points, faceCounts, faceConnects,
			newOutputData, &returnStatus);

		McheckErr(returnStatus, "ERROR creating new mesh");

		outputHandle.set(newOutputData);
		data.setClean( plug );
	} else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

