# [START program]
# [START import]

import sys
import ast
import os

sys.path.insert(0,"abanca_env/lib/python3.12/site-packages/")
from ortools.linear_solver import pywraplp

def create_data_model(matrixData, RPiData, PCjData):
    """Stores the data for the problem."""
    data = {}
    data["matrix"] = matrixData
    #print("\nmatrix type: ", type(data["matrix"]))
    #print(data["matrix"])
    
    data["RPi"] = RPiData #Purchase prices
    #print("RPi type: ", type(data["RPi"]))
    #print(data["RPi"])
    
    data["PCj"] = PCjData #Ask prices
    #print("PCj type: ", type(data["PCj"]))
    #print(data["PCj"])
    
    data["bounds"] = []
    for i in range(len(data["matrix"][0])):
        data["bounds"].append(1)    
    data["obj_coeffs"] = []
    
    for j in range(len(data["matrix"])):
        sumprod = 0
        for i in range(len(data["matrix"][0])):
            sumprod += data["matrix"][j][i] * data["RPi"][i]
        data["obj_coeffs"].append(sumprod - data["PCj"][j])


    data["num_vars"] = len(data["PCj"])
    data["num_constraints"] = len(data["RPi"])
    return data

def main():
    if(len(sys.argv)>2):
        #data = create_data_model(eval('"[[' + (sys.argv[1].replace(";","],[")) + ']]"'), eval('"[' + (sys.argv[2]) + ']"'), eval('"[' + (sys.argv[3]) + ']"'))
        # print("length of input = ",len(sys.argv))
        _matrixData = ast.literal_eval(eval('"[[' + (sys.argv[1].replace(":","],[")) + ']]"'))
        _RPiData = ast.literal_eval(eval('"[' + (sys.argv[2]) + ']"'))
        _PCjData = ast.literal_eval(eval('"[' + (sys.argv[3]) + ']"'))
        
        data = create_data_model(_matrixData, _RPiData, _PCjData)
        #print(data)
        # Create the mip solver with the SCIP backend.
        solver = pywraplp.Solver('simple_mip_program',pywraplp.Solver.CBC_MIXED_INTEGER_PROGRAMMING)  # pywraplp.Solver.CreateSolver("SCIP")
        if not solver:
            return

        # Define the variables
        x = {}
        for j in range(data["num_vars"]):
            x[j] = solver.IntVar(0, 1, "x[%i]" % j)
        #print(x)
        print("Number of variables =", solver.NumVariables())

        # Define the constraints
        for i in range(data['num_constraints']):
            constraint_expr = \
            [data['matrix'][j][i] * x[j] for j in range(data['num_vars'])]
            solver.Add(sum(constraint_expr) <= data['bounds'][i])
        # for i in range(data["num_constraints"]):
        #     constraint = solver.RowConstraint(0, data["bounds"][i], "")
        #     for j in range(data["num_vars"]):
        #         constraint.SetCoefficient(x[j], data["matrix"][i][j])
        print("Number of constraints =", solver.NumConstraints())

        # Define the objective
        obj_expr = [data['obj_coeffs'][j] * x[j] for j in range(data['num_vars'])]
        solver.Maximize(solver.Sum(obj_expr))

        # Call the solver
        status = solver.Solve()

        #Print the solution
        value_vars = []
        value_constraints = []
        if status == pywraplp.Solver.OPTIMAL:
            print("Objective value =", solver.Objective().Value())
            for j in range(data["num_vars"]):
                print(x[j].name(), " = ", x[j].solution_value())
                value_vars.append(x[j].solution_value())
            # Printing the value of constraints and variables:
            for i in range(data['num_constraints']):
                lhs_value = sum(data['matrix'][j][i]*x[j].solution_value() for j in range(data['num_vars']))
                value_constraints.append(lhs_value)
            print(";",value_constraints)
            print(":",value_vars)
            #print("Problem solved in %f milliseconds" % solver.wall_time())
            #print("Problem solved in %d iterations" % solver.iterations()) #This is a test
            #print("Problem solved in %d branch-and-bound nodes" % solver.nodes())
        else:
            print("The problem does not have an optimal solution.")

    else:
        print("0")

if __name__ == "__main__":
    main()