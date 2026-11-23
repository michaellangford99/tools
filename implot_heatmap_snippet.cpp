#include <implot.h>

void ShowHeatmap()
{
    constexpr int rows = 3;
    constexpr int cols = 4;

    // Row-major data: each group of `cols` values is one row.
    const float values[rows * cols] = {
        0.10f, 0.25f, 0.50f, 0.75f,
        0.20f, 0.40f, 0.65f, 0.90f,
        0.30f, 0.55f, 0.80f, 1.00f,
    };

    if (ImPlot::BeginPlot("2D heatmap", ImVec2(-1, 0))) {
        ImPlot::SetupAxes("X", "Y");
        ImPlot::PlotHeatmap(
            "Value",
            values,
            rows,
            cols,
            0.0f,
            1.0f,
            nullptr,
            ImPlotPoint(0, 0),
            ImPlotPoint(cols, rows));
        ImPlot::EndPlot();
    }
}
