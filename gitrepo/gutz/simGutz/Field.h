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

#ifndef _FIELD_H
#define _FIELD_H

#include <typelistGutz.h>

namespace gutz
{
   
   //////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////
   template<class PARTICLE>
   class P2PField
   {
   public:
      typedef PARTICLE particle_type;
      typedef typename PARTICLE::vec_type vec_type;
      typedef typename vec_type::value_type value_type;
      
      ///////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////
      P2PField(void)
      : _G       (static_cast<value_type>(6.67e-11))
      {}
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Calculate force exerted on p0 by p1
      ///
      /// @param p0
      ///   Particle 0
      ///
      /// @param p1
      ///   Particle 1
      ///
      /// @return
      ///       The acceleration vector. Units: m/s^2
      ///////////////////////////////////////////////////////////////////////////////////////
      vec_type operator()(const particle_type& p0, const particle_type& p1)
      {
         value_type m0 = p0.getMass();
         value_type m1 = p1.getMass();
         value_type dist;
         vec_type force(static_cast<value_type>(0));
         
         // Vector pointing from particle to singularity
         force = p1.getPosition() - p0.getPosition();
         
         // Find the distance between the particle and the singularity
         dist = force.norm();
         
         // If distance is zero, don't do the computation, return zero.
         if(dist <= static_cast<value_type>(0)) return vec_type(static_cast<value_type>(0));
         
         // Determine the magnitude of the gravitational force
         value_type g = secondLaw(m0, m1, dist);
//         std::cout << "pos0, pos1, m0, m1, g, dist: " << p0.getPosition() << "," << p1.getPosition() << m0 << "," << m1 << "," << g << "," << dist << std::endl;
         // Scale the force by the magnitude / distance. Normalizes and scales
         // in a single step.
         force *= (g / dist);
         
         // Change accumulated force into acceleration via F/m = a
         vec_type acceleration = force / m0;
         
         return acceleration;
      }
      
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Newton's 2nd Law of Gravitation.
      ///
      /// @param m1
      ///   mass of particle 1
      ///
      /// @return
      ///      Magnitude of the force. Units: Newton * meters^2 / kg^2
      ///////////////////////////////////////////////////////////////////////////////////////
      value_type secondLaw(value_type m1, value_type m2, value_type r)
      {
         return _G * m1 * m2 / (r * r);
      }
      
      
   private:
      // Keepin it real with Newton's 2nd Law of Gravitation
      value_type _G;
   };
   
   //////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////
   template<class P2PField>
   class ForEachParticlePair
   {
   public:
      typedef typename P2PField::vec_type vec_type;
      typedef typename P2PField::particle_type particle_type;
      
      ///////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////
      template<typename p_iterator_type>
      void operator()(vec_type& acceleration, p_iterator_type& p0i, const p_iterator_type& begin,
                      const p_iterator_type& end, P2PField& field)
      {
         particle_type p0 = *p0i;
         for(p_iterator_type i = begin; i != end; ++i)
         {
            // Don't operate on yourself
            if(p0i != i)
            {
               particle_type p1 = *i;
               acceleration += field(p0, p1);
            }
         }
      }
   };

   // No-op. This should get compiled out.
   template<>
   class ForEachParticlePair<NullType>
   {
   public:
      template<typename p_iterator_type, typename vec_type, typename P2PField>
      void operator()(vec_type& force, p_iterator_type& p0i, const p_iterator_type& begin, const p_iterator_type& end,
                      const P2PField& field)
      {
      }
   };
}

#endif

