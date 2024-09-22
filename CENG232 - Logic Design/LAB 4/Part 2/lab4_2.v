`timescale 1ns / 1ps
module SelectionOfAvatar(
	input [1:0] mode,
	input [5:0] userID,
	input [1:0] candidate, // 00:Air 01:Fire, 10:Earth, 11: Water
	input CLK,
	output reg [1:0] ballotBoxId,
	output reg [5:0] numberOfRegisteredVoters,
	output reg [5:0] numberOfVotesWinner, // number of votes of winner
	output reg [1:0] WinnerId,
	output reg AlreadyRegistered,
	output reg AlreadyVoted,
	output reg NotRegistered,
	output reg VotingHasNotStarted,
	output reg RegistrationHasEnded
	);
	integer i, counter;
	
	reg [5:0] voterList[63:0];
	reg [5:0] vote[3:0];
	reg [5:0] votedUsers[63:0];
	
	initial begin
		for(i = 0; i < 64; i = i+1)
		begin
			voterList[i] = 0;
			votedUsers[i] = 0;
		end
		for(i = 0; i < 4; i = i+1)
			vote[i] = 0;
		AlreadyRegistered = 0;
		VotingHasNotStarted = 0;
		NotRegistered = 0;
		AlreadyVoted = 0;
		RegistrationHasEnded = 0;
		WinnerId = 0;
		numberOfVotesWinner = 0;
		numberOfRegisteredVoters = 0;
		counter = 0;
		ballotBoxId = 0;
	end

always @(posedge CLK)
	begin
	AlreadyRegistered = 0;
	VotingHasNotStarted = 0;
	NotRegistered = 0;
	AlreadyVoted = 0;
	RegistrationHasEnded = 0;
	counter = counter + 1;
	ballotBoxId = userID[5:4];
	if (counter <= 100)
		begin
		if(mode == 0)
			begin
				if(voterList[userID] == 1)
					AlreadyRegistered = 1;
				else
					begin
					voterList[userID] = 1;
					numberOfRegisteredVoters = numberOfRegisteredVoters + 1;
					end
			end
		else
			begin
			VotingHasNotStarted = 1;
			end
		end
	else if(counter <= 200)
		begin
		if(mode == 1)
			begin
			if(voterList[userID] == 1)
				begin
				if (votedUsers[userID] == 0)
					begin
					vote[candidate] = vote[candidate] + 1'b1;
					votedUsers[userID] = 1;
					end
				else
					AlreadyVoted = 1;
				end
			else
				NotRegistered = 1;
			end
			else
				RegistrationHasEnded = 1;
		end
	else
		begin
		for(i = 0; i < 4; i = i + 1)
			begin
			if(vote[i] > numberOfVotesWinner)
				begin
				numberOfVotesWinner = vote[i];
				WinnerId = i;
				end
			end
		end
		
	end

endmodule
