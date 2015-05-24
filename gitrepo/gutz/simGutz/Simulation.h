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

#pragma once

#include <vector>
#include <Field.h>

namespace gutz
{
   template<typename INTEGRATOR, typename NBODY_FIELD=gutz::NullType>
   class Simulation
   {
   public:
      typedef INTEGRATOR integrator_type;
      typedef typename integrator_type::field_type    field_type;
      typedef typename field_type::particle_type      particle_type;
#if 1
      typedef NBODY_FIELD                    p2p_field_type;
#else
      typedef typename gutz::NullType                 p2p_field_type;
#endif
      typedef typename particle_type::vec_type        vec_type;
      typedef typename vec_type::value_type           value_type;
      typedef typename std::vector<particle_type>     particle_vector_type;
      typedef typename particle_vector_type::iterator particle_iterator;
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Constructor
      ///////////////////////////////////////////////////////////////////////////////////////
      Simulation(integrator_type* integrator, p2p_field_type* p2pField=0)
      : _integrator(integrator), _p2pField(0)
      {}
      
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Assignment operator
      ///////////////////////////////////////////////////////////////////////////////////////
      Simulation& operator=(const Simulation& sim)
      {
         if(this != &sim)
         {
            _integrator = sim._integrator;
            for(unsigned int i = 0; i < sim._particles.size(); ++i)
            {
               _particles.push_back(sim._particles[i]);
            }
         }
         
         return *this;
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      ///////////////////////////////////////////////////////////////////////////////////////
      virtual ~Simulation() {}
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Add a particle to the particle system
      ///
      /// @param particle
      ///   The particle to add
      ///////////////////////////////////////////////////////////////////////////////////////
      void addParticle(const particle_type& particle)
      {
         _particles.push_back(particle);
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////
      //   const vector<particle_type>& getParticles(void) const { return _particles; }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Get a reference to a particle in the particle system
      ///
      /// @param i
      ///   The index of the particle. No bounds checking is performed on this value
      ///
      /// @return a reference to a particle
      ///////////////////////////////////////////////////////////////////////////////////////
      particle_type& operator[](unsigned int i) { return _particles[i]; }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// @return the number of particles in the particle system
      ///////////////////////////////////////////////////////////////////////////////////////
      const size_t size(void) const { return _particles.size(); }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Advance the state
      ///////////////////////////////////////////////////////////////////////////////////////
      void advanceState(value_type time, value_type deltaTime)
      {
#if 0
         // Update each particle
         for(unsigned int i = 0; i < _particles.size(); ++i)
         {
            _particles[i] = (*_integrator)(_particles[i], time, deltaTime);
         }
#else
         ForEachParticlePair<p2p_field_type> fepp;
         //p2p_field_type p2p_field;
         vec_type acceleration;
         for(particle_iterator i = _particles.begin(); i != _particles.end(); ++i)
         {
//            ForEachParticlePair fepp;
            acceleration = vec_type(static_cast<value_type>(0));
            fepp(acceleration, i, _particles.begin(), _particles.end(), (*_p2pField));
//            std::cout << "acceleration: " << acceleration << std::endl;
            
            vec_type move(acceleration * (deltaTime * deltaTime));
            (*i) = (*_integrator)(*i, time, deltaTime);
            (*i).setPosition((*i).getPosition() + move);
//            (*i).setPosition((*i).getPosition() + move * 0.0);
//            std::cout << "move: " << move << std::endl;
         }
         
#endif
         
      }

   protected:
      Simulation() {}

      ///////////////////////////////////////////////////////////////////////////////////////
      /// Copy constructor
      ///////////////////////////////////////////////////////////////////////////////////////
      Simulation(const Simulation& sim)
      : _integrator(sim._integrator)
      {
         for(unsigned int i = 0; i < sim._particles.size(); ++i)
         {
            _particles.push_back(sim._particles[i]);
         }
      }
      
      
   private:
      particle_vector_type _particles;
      integrator_type*      _integrator;
      p2p_field_type*       _p2pField;
   };
   
}
