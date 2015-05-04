#ifndef CreateLSystemNode_H_
#define CreateLSystemNode_H_


#include <maya/MPxNode.h>


class LSystemNode : public MPxNode
{
public:
public:
	LSystemNode() {};
	virtual 		~LSystemNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();

	static MObject	time;
	static MObject	outputMesh;
	static MObject  angle;
	static MObject steps;
	static MObject grammar;
	static MTypeId	id;

};
#endif