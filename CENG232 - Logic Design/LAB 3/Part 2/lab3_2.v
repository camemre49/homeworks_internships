`timescale 1ns / 1ps
module lab3_2(
			input[5:0] money,
			input CLK,
			input vm, //0:VM0, 1:VM1
			input [2:0] productID, //000:sandwich, 001:chocolate, 11x: dont care
			input sugar, //0: No sugar, 1: With Sugar
			output reg [5:0] moneyLeft,
			output reg [4:0] itemLeft,
			output reg productUnavailable,//1:show warning, 0:do not show warning
			output reg insufficientFund , //1:full, 0:not full
			output reg notExactFund , //1:full, 0:not full
			output reg invalidProduct, //1: empty, 0:not empty
			output reg sugarUnsuitable, //1: empty, 0:not empty
			output reg productReady	//1:door is open, 0:closed
	);

	// Internal State of the Module
	// (you can change this but you probably need this)
	reg [4:0] numOfSandwiches;
	reg [4:0] numOfChocolate;
	reg [4:0] numOfWaterVM1;
	reg [4:0] numOfWaterVM2;
	reg [4:0] numOfCoffee;
	reg [4:0] numOfTea;

	initial
	begin
		// You can initialize here
    	// ...
		numOfSandwiches = 5'b01010;
		numOfChocolate = 5'b01010;
		numOfWaterVM1 = 5'b00101;
		numOfWaterVM2 = 5'b01010;
		numOfCoffee = 5'b01010;
		numOfTea = 5'b01010;
		moneyLeft = 6'b000000;
	end

	//Modify the lines below to implement your design
	always @(posedge CLK)
	begin
		productUnavailable = 1'b0;//1:show warning, 0:do not show warning
		insufficientFund  = 1'b0; //1:full, 0:not full
		notExactFund = 1'b0; //1:full, 0:not full
		invalidProduct = 1'b0; //1: empty, 0:not empty
		sugarUnsuitable = 1'b0; //1: empty, 0:not empty
		productReady = 1'b0; //1:door is open, 0:closed
		// You can implement your code here
    	// ...
		//VM0
		if(vm == 1'b0)
			begin
			if(productID == 3'b000)
				begin
				if(numOfSandwiches != 5'b0000)
					begin
					if(money == 6'b010100)
						begin
						numOfSandwiches = numOfSandwiches - 1'b1;
						itemLeft = numOfSandwiches;
						productReady = 1'b1;
						moneyLeft = 6'b000000;
						end
					else
						begin
						notExactFund = 1'b1;
						moneyLeft = money;
						end
					end
				else
					begin
					productUnavailable = 1'b1;
					moneyLeft = money;
					end
				end
			else if(productID == 3'b001)
				begin
					if(numOfChocolate != 5'b0000)
						begin
						if(money == 6'b001010)
							begin
							numOfChocolate = numOfChocolate - 1'b1;
							itemLeft = numOfChocolate;
							productReady = 1'b1;
							moneyLeft = 6'b000000;
							end
						else
							begin
							notExactFund = 1'b1;
							moneyLeft = money;
							end
						end
					else
						begin
						productUnavailable = 1'b1;
						moneyLeft = money;
						end
				end
			else if(productID == 3'b010)
				begin
					if(numOfWaterVM1 != 5'b0000)
						begin
						if(money == 6'b000101)
							begin
							numOfWaterVM1 = numOfWaterVM1 - 1'b1;
							itemLeft = numOfWaterVM1;
							productReady = 1'b1;
							moneyLeft = 6'b000000;
							end
						else
							begin
							notExactFund = 1'b1;
							moneyLeft = money;
							end
						end
					else
						begin
						productUnavailable = 1'b1;
						moneyLeft = money;
						end
				end
			else
				begin
				invalidProduct = 1'b1;
				moneyLeft = money;
				end
			end
			
		// VM1
		else
			begin
			if(productID == 3'b010)
				begin
				if(numOfWaterVM2 != 5'b00000)
					begin
					if (sugar == 1'b1)
						begin
						sugarUnsuitable = 1'b1;
						moneyLeft = money;
						end
					else
						begin
						if(money >= 6'b000101)
							begin
							numOfWaterVM2 = numOfWaterVM2 - 1'b1;
							itemLeft = numOfWaterVM2; 
							moneyLeft = money - 6'b000101;
							productReady = 1'b1;
							end
						else
							begin
							insufficientFund = 1'b1;
							moneyLeft = money;
							end
						end
					end
				else
					begin
					productUnavailable = 1'b1;
					moneyLeft = money;
					end
				end
			else if(productID == 3'b011)
				begin
				if(numOfCoffee != 5'b00000)
					begin
					if(money >=  6'b001100)
						begin
						numOfCoffee = numOfCoffee - 1'b1;
						itemLeft = numOfCoffee;
						moneyLeft = money - 6'b001100;
						productReady = 1'b1;
						end
					else
						begin
						insufficientFund = 1'b1;
						moneyLeft = money;
						end
					end
				else
					begin
					productUnavailable = 1'b1;
					moneyLeft = money;
					end
				end
			else if(productID == 3'b100)
				begin
				if(numOfTea != 5'b00000)
					begin
					if(money >=  6'b001000)
						begin
						numOfTea = numOfTea - 1'b1;
						itemLeft = numOfTea;
						moneyLeft = money - 6'b001000;
						productReady = 1'b1;
						end
					else
						begin
						insufficientFund = 1'b1;
						moneyLeft = money;
						end
					end
				else
					begin
					productUnavailable = 1'b1;
					moneyLeft = money;
					end
				end
			else
				begin
				invalidProduct = 1'b1;
				moneyLeft = money;
				end
		end
	end
endmodule