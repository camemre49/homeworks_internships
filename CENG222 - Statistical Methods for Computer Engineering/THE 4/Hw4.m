Probability = 0.98;
margin = 0.03;
SizeofMonteCarlo = 0.25*(norminv(1-((1-Probability)/2)) / margin)^2;
SizeofMonteCarlo = ceil(SizeofMonteCarlo);

Experiment_Array = zeros(SizeofMonteCarlo,1);
for k = 1:SizeofMonteCarlo

BulkCarrier_Poisson_lambda = 50;
BulkCarrier_Gamma_lambda = 0.1;
BulkCarrier_Alpha = 60;
BulkCarrier_Count = CountTheVehicle(BulkCarrier_Poisson_lambda);
BulkCarrier_Tons = GammaCalculation(BulkCarrier_Gamma_lambda, BulkCarrier_Alpha, BulkCarrier_Count);

ContainerShip_Poisson_lambda = 40;
ContainerShip_Gamma_lambda = 0.05;
ContainerShip_Alpha = 100;
ContainerShip_Count = CountTheVehicle(ContainerShip_Poisson_lambda);
ContainerShip_Tons = GammaCalculation(ContainerShip_Gamma_lambda, ContainerShip_Alpha, ContainerShip_Count);
 
OilTankers_Poisson_lambda = 25;
OilTankers_Gamma_lambda = 0.02;
OilTankers_Alpha = 120;
OilTankers_Count = CountTheVehicle(OilTankers_Poisson_lambda);
OilTankers_Tons = GammaCalculation(OilTankers_Gamma_lambda, OilTankers_Alpha, OilTankers_Count);

TotalWeight = OilTankers_Tons + ContainerShip_Tons + BulkCarrier_Tons;
Experiment_Array(k) = TotalWeight;
end
AverageCargoinDay = mean(Experiment_Array);
STD = std(Experiment_Array);

DesiredTons = 300000;
ProbabilityMorethan = 1 - normcdf((DesiredTons - AverageCargoinDay)/STD);

fprintf("Estimated probability: %f\n", ProbabilityMorethan);
fprintf("Expected weight: %f\n", AverageCargoinDay);
fprintf("Standard deviation: %f\n", STD);


function Count = CountTheVehicle(lambda)
U = rand;
i = 0;
F = exp(-lambda);
while (U >= F)
    i = i + 1;
    F = F + exp(-lambda) * lambda^i/gamma(i+1);
end
Count = i;
end

function Tons = GammaCalculation(lambda, alpha, numberOfVehicles)
Tons = 0;
for i = 1:numberOfVehicles
X = sum( -1/lambda * log(rand(alpha,1)) );
Tons = Tons + X;
end
end