/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

/* -*- mode: c-non-suck; -*- */

#ifndef CONTROL_BASIS_PRIMITIVE_CONTROLLER_HH
#define CONTROL_BASIS_PRIMITIVE_CONTROLLER_HH

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <vector>

#include <cbf/controller.h>
#include <cbf/convergence_criterion.h>
#include <cbf/potential.h>
#include <cbf/resource.h>
#include <cbf/effector_transform.h>
#include <cbf/reference.h>
#include <cbf/sensor_transform.h>
#include <cbf/combination_strategy.h>
#include <cbf/namespace.h>

namespace CBFSchema { 
	class PrimitiveController; 
	class SubordinateController;
}

namespace CBF {
	class SubordinateController;
	typedef boost::shared_ptr<SubordinateController> SubordinateControllerPtr;

	class PrimitiveController;
	typedef boost::shared_ptr<PrimitiveController> PrimitiveControllerPtr;
	

	struct SubordinateController : public Controller {

		friend class TaskSpaceDistanceThreshold;
		friend class ResourceStepNormThreshold;

		SubordinateController(const CBFSchema::SubordinateController &xml_instance, ObjectNamespacePtr object_namespace);

		/**
			@brief Create a controller with the members 
			set from the specified arguments
		*/
		SubordinateController(
			SubordinateController *master,
			Float coefficient,
			std::vector<ConvergenceCriterionPtr> convergence_criteria,
			ReferencePtr reference,
			PotentialPtr potential,
			SensorTransformPtr sensor_transform,
			EffectorTransformPtr effector_transform,
			std::vector<SubordinateControllerPtr> subordinate_controllers,
			CombinationStrategyPtr combination_strategy
		);
	
	
		protected:
			SubordinateController* m_Master;
			bool m_Converged;

			/**
				@brief: Check for convergence of the controller
		
				This should only be used after the result has been calculated
			*/ 
			virtual bool check_convergence();

			std::vector<ConvergenceCriterionPtr> m_ConvergenceCriteria;
	
			/*** @brief Function for stuff common to all constructors */
			void init(
				SubordinateController* master,
				Float coefficient,
				std::vector<ConvergenceCriterionPtr> convergence_criteria,
				ReferencePtr reference,
				PotentialPtr potential,
				SensorTransformPtr sensor_transform,
				EffectorTransformPtr effector_transform,
				std::vector<SubordinateControllerPtr> subordinate_controllers,
				CombinationStrategyPtr combination_strategy
			);

			/**
				A reference determines the task 
				space position that the controller
				tries to realize. Sometimes there 
				can be more than one possible 
				target. It's left to the controller 
				(by way of the potential
				function) which of the targets to reach..
			*/
			ReferencePtr m_Reference;
	
			/**
				A controller can have subordinate controllers whose control signal
				get projected into the nullspace of the task jacobian
			*/
			std::vector<SubordinateControllerPtr> m_SubordinateControllers;
	
			/**
				The sensor transform is responsible for providing the feedback
				of this closed loop controller. See SensorTransform..
		
				The derivative of the sensor transform is the task jacobian. 
				Its inverse will be represented by the effector transform..
			*/
			SensorTransformPtr m_SensorTransform;
	
			/**
				The potential is the "heart" of the controller. 
			*/
			PotentialPtr m_Potential;
	
			/**
				The effector transform is responsible for mapping the 
				gradient step output of the controller to a real actuator.
		
				In many cases it will implement the pseudoinverse jacobian 
				of the sensortransform..
			*/
			EffectorTransformPtr m_EffectorTransform;
	
			CombinationStrategyPtr m_CombinationStrategy;

			/**
				The factor for the primary gradient step
			*/
			Float m_Coefficient;

		public:
			/**
				This function only returns sensible values after
				the controller has run at least once...
			*/
			const FloatVector &current_task_position() 
				{ return m_CurrentTaskPosition; }

			ReferencePtr reference() 
				{ return m_Reference; }
	
			std::vector<SubordinateControllerPtr> &subordinate_controllers() 
				{ return m_SubordinateControllers; }
	
			SensorTransformPtr sensor_transform() 
				{ return m_SensorTransform; }
	
			PotentialPtr potential() 
				{ return m_Potential; }
	
			EffectorTransformPtr effector_transform() 
				{ return m_EffectorTransform; }
	
			CombinationStrategyPtr combination_strategy() 
				{ return m_CombinationStrategy; }
	
			Float coefficient();
	
		
			/**
				This reimplementation of the base class' method assumes that we are not a subordinate
				controller, because subordinate controllers are always called via the do_step() method
			*/
			virtual void update();

			virtual void action() { }

			/**
				@brief Returns the result of the calculationss done on update().

				The update() function is supposed to update all sensor transforms, potentials
				etc. The last step of a control cycly is applying the result (the resource
				space gradient step) of these	calculations to the resource.
			*/
			virtual FloatVector &result() { return m_Result; }

			virtual ResourcePtr resource();

			/**

				Check if controller is converged. Call this function only
				after calling step() at least once..
			*/
			virtual bool finished();

			virtual void check_dimensions();

		protected:
			FloatVector m_Result;

			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_TaskJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_InverseTaskJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatMatrix m_InvJacobianTimesJacobian;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_CurrentTaskPosition;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_GradientStep;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_ResourceStep;
	
			/**	Member variable for efficiency reasons.. */
			FloatVector m_CombinedResults;
	
			/**	Member variable for efficiency reasons.. */
			std::vector<FloatVector> m_SubordinateGradientSteps;
	
			/**	Member variable for efficiency reasons.. */
			std::vector<FloatVector> m_References;
	};



	/**
		@brief The "primitive" controller is the 
		main tool to use in the control basis framework.
	
		A primitive controller can be "synthesized" 
		by combining an artificial potential function,
		a sensor transform, an effector transform 
		and a resource to act on.
	
		Additionally several subordinate controllers 
		can be specified which will try to reach 
		secondary goals. The result of the gradient 
		steps of the subordinate controller is 
		projected into the null space of this controller.
	*/
	struct PrimitiveController : public SubordinateController {

		PrimitiveController(const CBFSchema::PrimitiveController &xml_instance, ObjectNamespacePtr object_namespace);

		/**
			@brief Create a controller with the members 
			set from the specified arguments
		*/
		PrimitiveController(
			Float coefficient,
			std::vector<ConvergenceCriterionPtr> convergence_criteria,
			ReferencePtr reference,
			PotentialPtr potential,
			SensorTransformPtr sensor_transform,
			EffectorTransformPtr effector_transform,
			std::vector<SubordinateControllerPtr> subordinate_controllers,
			CombinationStrategyPtr combination_strategy,
			ResourcePtr resource
		);
	
	
		protected:
			bool m_Converged;

			/*** @brief Function for stuff common to all constructors */
			void init(ResourcePtr resource);

			/**
				A controller can have subordinate controllers whose control signal
				get projected into the nullspace of the task jacobian
			*/
			std::vector<PrimitiveControllerPtr> m_SubordinateControllers;
	

			/**
				The resource this controller acts upon
			*/
			ResourcePtr m_Resource;

			virtual void check_dimensions();

		public:

			virtual ResourcePtr resource() { return m_Resource; }

			virtual void update();

			virtual void action();
	};

} // namespace



#endif

