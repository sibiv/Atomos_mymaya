#include "LSystemCmd.h"
#include "LSystem.h"

#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <list>

const char *ssFlag = "-ss",*stepsizeFlag = "-step";
const char *aFlag = "-a",*angleFlag = "-angle";
const char *iFlag = "-i",*iterationsFlag="-iterflag";
const char *gFlag = "-g",*grammarFlag = "-grammar";

LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MStatus LSystemCmd::doIt( const MArgList& args )
{
	//cout<<"Implement Me!"<<endl;
	double start_x = 0;
	double start_y=0,start_z=0,end_x=5,end_y=5,end_z=5;
	MString x = "-d";
	MString p = "-p";
	MString curve1 = "curve";
	double step_size = 0;
	double iterations = 0;
	double angleindeg = 0;

	int sum =0;

	MString grammar = "F\\nF+F";

	MArgDatabase argData( newsyntax(), args );

	if( argData.isFlagSet( ssFlag ) )
		argData.getFlagArgument( ssFlag, 0, step_size );

	if( argData.isFlagSet( aFlag ) )
		argData.getFlagArgument( aFlag, 0, angleindeg );

	if( argData.isFlagSet( iFlag ) )
		argData.getFlagArgument( iFlag, 0, iterations );

	if( argData.isFlagSet( gFlag ) )
		argData.getFlagArgument( gFlag, 0, grammar );

	LSystem system;
	system.loadProgramFromString(grammar.asChar());
	system.setDefaultAngle(angleindeg);
	system.setDefaultStep(step_size);


	for (int i = 0; i < iterations; i++)
	{
		std::string insn = system.getIteration(i);
		std::cout << insn << std::endl;

		std::vector<LSystem::Branch> branches;
		system.process(i, branches);

		for (int j=0; j<branches.size();j++)
		{
			sum++;
			curve1 = "curve";
			curve1 += sum;
			MString mycommand = ("curve "+ x +" 1 -p "+ branches[j].first[0]+" "+branches[j].first[1]+" "+branches[j].first[2]);
			mycommand += (p+" "+ branches[j].second[0]+" "+branches[j].second[1]+" "+branches[j].second[2]+" -k 0 -k 1 -name "+ curve1 +";select -r nurbsCircle1 "+curve1+";extrude-ch true -rn false -po 1 -et 2 -ucp 1 -fpt 1 -upn 1 -rotation 0 -scale 1 -rsp 1 \"nurbsCircle1\" \""+curve1+"\" ;");
			MGlobal::executeCommand(mycommand);
		}



	}
	MGlobal::displayInfo("Implement Me! gram:"+grammar + " step:"+ step_size +" angleindeg" + angleindeg +" iterations" + iterations);
	return MStatus::kSuccess;
}

MSyntax LSystemCmd::newsyntax()
{
	MSyntax a;
	a.addFlag( ssFlag, stepsizeFlag, MSyntax::kDouble );
	a.addFlag( aFlag, angleFlag, MSyntax::kDouble );
	a.addFlag(iFlag,iterationsFlag,MSyntax::kDouble);
	a.addFlag( gFlag, grammarFlag, MSyntax::kString );
	return a;
}
