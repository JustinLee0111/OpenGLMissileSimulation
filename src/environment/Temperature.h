#pragma once

struct Temperature {
	Temperature() = default;
	// Temp transfer ignores surface area, uses this only to control transfer rate
	float temperature{ 400.0f };
	float tempTransferCoef{ 0.5f }; // Higher coefficient = more heat transfer
};