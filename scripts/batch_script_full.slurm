#!/bin/bash
#SBATCH --time=01:20:00
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=16
#SBATCH --exclusive
#SBATCH --job-name parRay-team7
#SBATCH -p cs
#PBS -S ./scripts/sing_shell.sh

echo `whoami`
## If not started with PBS, figure out where we are relative to the build directory
#####Snippet from:   http://stackoverflow.com/questions/59895/
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#####end snippet
echo $SCRIPT_DIR
#IF SLURM_SUBMIT_DIR is not set, we are not running in PBS, choose directory relative to script.
SLURM_SUBMIT_DIR=${SLURM_SUBMIT_DIR:-${SCRIPT_DIR}/..}
echo $SLURM_SUBMIT_DIR
#moves to the directory the user was in when they ran qsub
cd ${SLURM_SUBMIT_DIR} #assumed to be the source tree

#check that the script was submit from the right place.
if [ -d "./src" ]
then
	echo "We seem to be in the right place."
else
	echo "Not submit from the right place! Submit from the root of your repository."
	exit 1
fi

if [ -d "./build" ]
then
	rm -rf ./build
fi

SING_EXEC_LOCATION=${SLURM_SUBMIT_DIR}/scripts/sing_exec.sh
SING_SH_LOCATION=${SLURM_SUBMIT_DIR}/scripts/sing_shell.sh
BIN_DIR=${SLURM_SUBMIT_DIR}/build/bin

#creates an out-of-tree build directory for CMake and moves to it
mkdir -p ${SLURM_SUBMIT_DIR}/build
pushd ${SLURM_SUBMIT_DIR}/build

echo "Compiling"
srun --mpi=pmi2 --ntasks 1 --nnode 1  ${SING_EXEC_LOCATION} cmake ${SLURM_SUBMIT_DIR}/src && \
srun --mpi=pmi2 --ntasks 1 --nnode 1  ${SING_EXEC_LOCATION} make

echo "Run Tests"
srun --mpi=pmi2 --ntasks 1 --nnode 1 ${SING_EXEC_LOCATION}  "${BIN_DIR}/bvh_test"
srun --mpi=pmi2 --ntasks 1 --nnode 1 ${SING_EXEC_LOCATION}  "${BIN_DIR}/data_porting_test"

echo "Run Benchmarks"
srun --mpi=pmi2 --ntasks 1 ${SING_EXEC_LOCATION}  "${BIN_DIR}/bm_ray_tracing"
srun --mpi=pmi2 --ntasks 1 --nnode 1 ${SING_EXEC_LOCATION}  "${BIN_DIR}/gen_random_scene 10"


for N_CPUS in 1 2 4 8 16
do

	#Individual MPI run
	echo "Doing MPI CPUS = ${N_CPUS} "
#	srun --mpi=pmi2 --ntasks-per-node ${SLURM_NTASKS_PER_NODE} --cpu-bind=cores --ntasks ${N_CPUS} ~/scratch/sing_exec.sh ./bin/bm_mpi ./random_spheres_scene.data 2
	srun --mpi=pmi2 --cpu-bind=cores --ntasks ${N_CPUS} ${SING_EXEC_LOCATION} "${BIN_DIR}/bm_mpi ./random_spheres_scene.data 2"
done

