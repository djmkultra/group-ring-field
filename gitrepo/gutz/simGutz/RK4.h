/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           
//                                          8MMNMM8                                          
//                                     $MMMMMMMMMMMNMMNNNNN:                                 
//                             7MMMMMMMMMMMMMNNNMNNNNNNNMNDDDDD7                             
//                            MMMMMMMMMMMMMMNNNNNNNNNNNNNND8DD888                            
//                          ZMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDD8888:                        
//                       8MMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDD88888=                       
//                     MMMMMMMMMMMMMMMMMMMMMMMNNNMNNNNNNNNNNNNNDD88888O8                     
//                   MMMMMMMMMMMMMMMMNNNNNNNNNNNDDDDDDDDDDNNDNNNDD888888O8                   
//                  MMMMMMMMMMMMMMMNNNNNNNNNNNNDDDDDDDDDDDDDDD888ND88888OO8                  
//                 MMMMMMMMMMMMMMMNNNNNNNNNNNNNNDDDDDDDDDDDDDDD888OO8DDOOZZZ                 
//                MMMMMMMMMMMMMMMNMMMMMMMMMMMMMMMMMMMMMMNDDDDDD8888OOOZZ$$$$Z                
//               ,MMMMMMMMMMMMMMMMMNNNNNMMMMMMMMMMMMMMMDDDDDDMMMDN8OOOOZZZ++I7               
//               MMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDDDDDDDMMM88OMMD8OOZZZ$~I               
//              MMMMMMMMMMMMMMMMMNND8Z7I+=~::::::::~~=?I$O88DDDDD8MMOO8M8OZZ$$$              
//             MMMMMMMMMMMMMNDOI+:,                         ,~+78O888OMMZDMO$$$$             
//            MMMMMMMMMMM87=,                                     ,+$8OOOMMZM8$$7            
//           MMMMMMMMMO?:                                             :IOZO8M7MO77           
//          MMMMMMMDI,                                                   :7ZZZM7MZ7          
//         MMMMMMD?,                                                       ,7DZMDMZ7         
//        :MMMMN?,                                                           ,$$$MMZ,        
//        MMMM8~                                                               +N$MM7        
//        MMMZ,                                                                 ~DZMM        
//        MMD:                                           8""""8 8                1XD         
//          #2IN                eeeee eeeee eeeeeee eeee 8    " 8               2YE          
//            1HM               8   8 8  88 8  8  8 8    8e     8e             3ZF           
//              0GL             8e  8 8   8 8e 8  8 8eee 88  ee 88            4AG            
//                9FK           88  8 8   8 88 8  8 88   88   8 88           5BH             
//                  8EJ         88ee8 8eee8 88 8  8 88ee 88eee8 88eee       6CJ              
//                    7DI                                                7DI                 
//                       OO?~                                        ,~78D                   
//                         D8$+:,                             ,~?ZDN                         
//                             ,NDO$?=::,             ,:~=?$8DM,                             
//                                       ~7NNNNN8NNNNNZ:                                     
//                                                                                           
//                                                                                           
//                                       Copyright 2011                                      
//                      Art, Research, Technology and Science Laboratory                     
//                                 The University of New Mexico                              
//                                      Project Home Page                                    
//                           <<<<http://artslab.unm.edu/domegl>>>>>                          
//                                       Code Repository                                     
//                           <<<https://svn.cs.unm.edu/domegl>>>>>>                          
//                                                                                           
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _RK4_h
#define _RK4_h

#include <vec.h>

/////////////////////////////////////////////////////////////////////////////////////////////
/// Runge-Kutta 4th Order Solver
///
/// This is a templatized class that requires a functor to find the "acceleration" (dv/dt) of a particle
/// at a particular time.
///
/// 
/// Example functor that implements gravity
/// \code
/// template <typename T = float>
/// class Gravity
/// {
/// public:
///    typedef T precision;             //< Precision of simulation. RK4 uses this to determine its precision
///    typedef gutz::vec<T,3> vec_type; //< gutz::vec type used for the simulation. RK4 uses this to determine to represent vectors
///    struct  State                    //< State of a particle. RK4 uses this data structure to pass around state.
///    {
///       vec_type x;                   //< Must have a public member named x for RK4 to work
///       vec_type v;                   //< Must have a public member named v for RK4 to work
///    };
///    
///    /// Constructor
///    Gravity(void) {}
///    
///    /// Newton's second law of gravitation
///    T secondLaw(T m1, T m2, T r)
///    {
///       static const T G = 6.67e-11f; // Nm^2/kg^2
///       return G * m1 * m2 / (r * r);
///    }
///    
///    /// The "functor" part of the functor. The RK4 needs a functor defined in
///    /// exactly this way to compile and function properly.
///    ///   
///    /// In this case, it calculates acceleration at this instant.
///    vec_type operator()(const State& state)
///    {
///       static T m1 = 1e0f; // Particle mass
///       static T m2 = 9.5e9f; // Singularity mass
///       
///       // Vector pointing from particle to singularity, assume singularity is at 0
///       vec_type force = vec_type(static_cast<T>(0)) - state.x;
///       
///       // Find the distance between the particle and the singularity
///       T dist = force.norm();
///       
///       // Determine the magnitude of the gravitational force
///       T g = secondLaw(m1, m2, dist);
///       
///       // Scale the force by the magnitude / distance. Normalizes and scales
///       // in a single step.
///       force *= (g / dist);
///       
///       // Convert force into acceleration via a = F/m
///       vec_type accel = force / m1;
///       return accel;
///    }
/// };
/// \endcode

namespace gutz
{

   template <class FIELD>
   class RK4
   {
   public:
      
      typedef FIELD field_type;
      typedef typename FIELD::particle_type      particle_type;
      typedef typename particle_type::value_type value_type;
      typedef typename particle_type::vec_type   vec_type;
      
      // dx/dt and dv/dt. Used to pass around derivatives.
      struct Derivative
      {
         vec_type dx;
         vec_type dv;
      };
      
      
      /// Constructor
      RK4(field_type* field)
      :  _field   (field)
      {}
      
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Integration step
      ///
      /// @params state    the current state for a particle
      ///         t        the current time
      ///         dt       size of the time step
      particle_type operator()(const particle_type& state, value_type t, value_type dt)
      {
         Derivative a = evaluate(state, t);
         Derivative b = evaluate(state, t, dt*0.5f, a);
         Derivative c = evaluate(state, t, dt*0.5f, b);
         Derivative d = evaluate(state, t, dt, c);
         static value_type one_sixth = 1.0f/6.0f; // What if we're using complex numbers? Will this work?
         const vec_type dxdt = (a.dx + (b.dx + c.dx) * 2.0f + d.dx) * one_sixth;
         const vec_type dvdt = (a.dv + (b.dv + c.dv) * 2.0f + d.dv) * one_sixth;
         
         //      std::cout << "state.x:             " << state.x << std::endl;
         //      std::cout << "state.x + dxdt * dt: " << state.x + dxdt * dt << std::endl;
         
         
         //      std::cout << "newState.x: " << newState.x << std::endl;
         return particle_type(state.getPosition() + dxdt * dt,
                              state.getVelocity() + dvdt * dt,
                              state.getMass());
      }
      
   protected:
      Derivative evaluate(const particle_type& initial, value_type t)
      {
         Derivative output;
         output.dx = initial.getVelocity();
         // Call _field's functor with particle_type's current state
         output.dv = (*_field)(initial);
         return output;
      }
      
      Derivative evaluate(const particle_type& initial, value_type t, value_type dt, const Derivative& d)
      {
         particle_type state(initial.getPosition() + d.dx * dt,
                             initial.getVelocity() + d.dv * dt,
                             initial.getMass());
         
         Derivative output;
         output.dx = state.getVelocity();
         output.dv = (*_field)(state);
         
         return output;
      }
      
   private:
      field_type* _field; //< Force field
   };

   namespace deprecated
   {
      
      template <typename FUNCTOR>
      class RK4
      {
      public:
         
         // Functor's precision, could be float, double, complex, etc.
         typedef typename FUNCTOR::precision PRECISION;
         
         // gutz::vec type used to represent position, velocity, and acceleration
         typedef typename FUNCTOR::vec_type vec_type;
         
         // State structure. Must have x and v members at a minimum
         typedef typename FUNCTOR::State State;
         
         // dx/dt and dv/dt. Used to pass around derivatives.
         struct Derivative
         {
            vec_type dx;
            vec_type dv;
         };
         
         
         /// Constructor
         RK4() {}
         
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         /// Integration step
         ///
         /// @params state    the current state for a particle
         ///         t        the current time
         ///         dt       size of the time step
         State operator()(const State& state, PRECISION t, PRECISION dt)
         {
            State newState;
            Derivative a = evaluate(state, t);
            Derivative b = evaluate(state, t, dt*0.5f, a);
            Derivative c = evaluate(state, t, dt*0.5f, b);
            Derivative d = evaluate(state, t, dt, c);
            static PRECISION one_sixth = 1.0f/6.0f; // What if we're using complex numbers? Will this work?
            const vec_type dxdt = (a.dx + (b.dx + c.dx) * 2.0f + d.dx) * one_sixth;
            const vec_type dvdt = (a.dv + (b.dv + c.dv) * 2.0f + d.dv) * one_sixth;
            
            //      std::cout << "state.x:             " << state.x << std::endl;
            //      std::cout << "state.x + dxdt * dt: " << state.x + dxdt * dt << std::endl;
            
            newState.x = state.x + dxdt*dt;
            newState.v = state.v + dvdt*dt;
            //      std::cout << "newState.x: " << newState.x << std::endl;
            return newState;
         }
         
         FUNCTOR* getFunctor(void) { return &_acceleration; }
         
      protected:
         Derivative evaluate(const State& initial, PRECISION t)
         {
            Derivative output;
            output.dx = initial.v;
            output.dv = _acceleration(initial);
            return output;
         }
         
         Derivative evaluate(const State& initial, PRECISION t, PRECISION dt, const Derivative& d)
         {
            State state;
            state.x = initial.x + d.dx * dt;
            state.v = initial.v + d.dv * dt;
            
            Derivative output;
            output.dx = state.v;
            output.dv = _acceleration(state);
            
            return output;
         }
         
      private:
         FUNCTOR _acceleration; //< Force field
      };
   }
}

#endif
