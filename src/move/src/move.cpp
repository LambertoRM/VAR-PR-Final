#include "ros/ros.h"
#include "std_msgs/Float64MultiArray.h"
#include "geometry_msgs/Twist.h"
#include <math.h>

class move{
	protected:
		ros::Subscriber infoSub;
		ros::Publisher velPub;

		double pos1;
		double pos2;
		double pos3;

		geometry_msgs::Twist speed;
		double PI = 3.1415926535897;
		double default_vel = 0.2;
		double default_vel_piv = 0.15;
		const int angle = 10; // Grados
		double relative_angle;

	public:
		move(ros::NodeHandle &nh){
	       pos1 = 0.0;
           pos2 = 0.0;
           pos3 = 0.0;
		   relative_angle = angle*(PI/180); // Pasamos de grados a radianes

		   infoSub = nh.subscribe("laserInfo", 1, &move::getLaserInfo, this);
		   velPub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);
		}
        
        //Obtenemos la informacion del laser
		void getLaserInfo(const std_msgs::Float64MultiArray::ConstPtr &msg){
	       for(size_t i = 0; i < msg->data.size(); i++){
		      std::cout << msg->data[i] <<" ";
		   }

	       std::cout << std::endl;

		   pos1 = msg->data[0];
		   pos2 = msg->data[1];
	       pos3 = msg->data[2];
		}
		
		
		void notMove(){
		   speed.linear.x = 0.0;
		   speed.angular.z = 0.0;
		}

		void straight(){
		   notMove();
		   speed.linear.x = default_vel;
		}
	
		void moveRight(){
		   notMove();
		   speed.linear.x = default_vel_piv;
		   speed.angular.z = -relative_angle;
		}

		void moveLeft(){
		   notMove();
		   speed.linear.x = default_vel_piv;
           speed.angular.z = relative_angle;
		}

		void pivoteRight(){
		   notMove();
		   speed.angular.z = -relative_angle;
		}
	
		void pivoteLeft(){
		   notMove();
		   speed.angular.z = relative_angle;
		}

		void driveRobot(){

	       if(pos1 >= 0.71 && pos1 <= 0.91 
		      && pos2 < 1.51){
			     std::cout << "RECTO " << pos1 << " " << pos2 << std::endl;
				 straight();
		   }

		   else{

		      if(pos3 <= 0.99) {
			     std::cout << "PIVOTEO IZQUIERDA " << pos3 << std::endl;
				 pivoteLeft();
			  }

			  else if(pos1 > 0.91){
			     std::cout << "DERECHA " << pos1<< std::endl;
				 moveRight();
			  }

			  else if(pos1 < 0.71){
			     std::cout << "IZQUIERDA " << pos1 << std::endl;
				 moveLeft();
		      }

			  else{
			     std::cout << "IZQUIERDA " << pos1 << std::endl;
	             moveLeft();
			  }
		   }
		}
        
        //Bucle principal
		void bucle(){
			ros::Rate rate(10);
			while(ros::ok()){
				geometry_msgs::Twist msg;
				driveRobot();
				msg.linear = speed.linear;
				msg.angular = speed.angular;								
				velPub.publish(msg);
				ros::spinOnce();
				rate.sleep();
			}
		}
};

int main(int argc, char** argv){
	ros::init(argc, argv, "move");
	ros::NodeHandle nh;
	move mv(nh);
	mv.bucle();
	return 0;
}