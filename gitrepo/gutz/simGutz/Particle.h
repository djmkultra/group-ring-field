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

#include <vec.h>

namespace gutz
{
   
   template<typename VEC_TYPE = gutz::vec3f>
   class Particle
   {
   public:
      typedef typename VEC_TYPE::value_type value_type;
      static const int dimension = VEC_TYPE::size;
      typedef VEC_TYPE vec_type;

      ///////////////////////////////////////////////////////////////////////////////////////
      /// Default constructor
      ///////////////////////////////////////////////////////////////////////////////////////
      Particle() 
      :  _id         (-1),
         _position   (vec_type(static_cast<value_type>(0))),
         _velocity   (vec_type(static_cast<value_type>(0))),
         _mass       (static_cast<value_type>(0))
      {
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Constructor
      ///////////////////////////////////////////////////////////////////////////////////////
      Particle(const vec_type& position, const vec_type& velocity, const value_type& mass,
               const int id = -1)
      :  _id         (id),
         _position   (position),
         _velocity   (velocity),
         _mass       (mass)
      {}
      

      ///////////////////////////////////////////////////////////////////////////////////////
      /// Copy constructor
      ///////////////////////////////////////////////////////////////////////////////////////
      Particle(const Particle& p0)
      :  _id            (p0._id),
         _position      (p0._position),
         _velocity      (p0._velocity),
         _acceleration	(p0._acceleration),
         _mass          (p0._mass),
         _charge        (p0._charge),
         _northPole     (p0._northPole),
         _guass         (p0._guass),
         _radius        (p0._radius),
         _age           (p0._age),
         _halfLife      (p0._halfLife)
      {
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Assignment operator
      ///////////////////////////////////////////////////////////////////////////////////////
      Particle& operator=(const Particle& p0)
      {
         if(&p0 != this)
         {
            _position      = p0._position;
            _velocity      = p0._velocity;
            _acceleration	= p0._acceleration;
            _mass          = p0._mass;
            _charge        = p0._charge;
            _northPole     = p0._northPole;
            _guass         = p0._guass;
            _radius        = p0._radius;
            _age           = p0._age;
            _halfLife      = p0._halfLife;
         }
         
         return *this;
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      ///////////////////////////////////////////////////////////////////////////////////////
      virtual ~Particle() {}
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// @return position of the particle in meters.
      ///////////////////////////////////////////////////////////////////////////////////////
      vec_type getPosition(void) const { return _position; }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// @return velocity of the particle in meters per second.
      ///////////////////////////////////////////////////////////////////////////////////////
      vec_type getVelocity(void) const { return _velocity; }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// @return acceleration in meters per second squared
      ///////////////////////////////////////////////////////////////////////////////////////
      vec_type getAcceleration(void) const { return _acceleration; }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Set the position of the particle.
      ///
      /// @param position
      ///   Position measured in meters
      ///////////////////////////////////////////////////////////////////////////////////////
      void setPosition(const vec_type& position)
      {
         _position = position;
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Set the velocity of the of the particle
      ///
      /// @param velocity
      ///   The velocity of the particle in meters per second
      ///////////////////////////////////////////////////////////////////////////////////////
      void setVelocity(const vec_type& velocity)
      {
         _velocity = velocity;
      }
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Set the acceleration of the particle
      ///
      /// @param acceleration
      ///   The acceleration of the particle in meters per second squared
      ///////////////////////////////////////////////////////////////////////////////////////
      void setAcceleration(const vec_type& acceleration)
      {
         _acceleration = acceleration;
      }

      ///////////////////////////////////////////////////////////////////////////////////////
      /// Set the mass of a particle
      ///////////////////////////////////////////////////////////////////////////////////////
      void setMass(const value_type& mass)
      {
         _mass = mass;
      }   

      ///////////////////////////////////////////////////////////////////////////////////////
      /// @return the mass of a particle
      ///////////////////////////////////////////////////////////////////////////////////////
      const value_type getMass(void) const
      {
         return _mass;
      }   
      
      
      ///////////////////////////////////////////////////////////////////////////////////////
      /// Get the id for this particle
      ///////////////////////////////////////////////////////////////////////////////////////
      const int getID(void) const
      {
         return _id;
      }

      ///////////////////////////////////////////////////////////////////////////////////////
      /// Set the id for this particle
      ///////////////////////////////////////////////////////////////////////////////////////
      void setID(const int id)
      {
         _id = id;
      }
      
   private:
      int          _id;              //< The particle id
      vec_type     _position;        //< Position
      vec_type     _velocity;        //< Velocity, first derivative
      vec_type     _acceleration;    //< Acceleration, second derivative
      value_type   _mass;            //< Kilograms
      value_type   _charge;          //< Coulombs
      vec_type     _northPole;       //< Direction of magnetic north on this particle
      value_type   _guass;           //< Strength of magnetic field
      value_type   _radius;          //< Radius in meters
      value_type   _age;             //< Particle's age in seconds
      value_type   _halfLife;        //< The half life of the particle in seconds
   };
   
   
};