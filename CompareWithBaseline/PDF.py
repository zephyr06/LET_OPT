import matplotlib.pyplot as plt
import numpy as np

# Sample method names and their corresponding performance scores
method_names = ['Method1', 'Method2', 'Method3', 'Method4', 'Method5',
                'Method6', 'Method7', 'Method8', 'Method9', 'Method10']

performance_scores = [90, 85, 80, 75, 70, 65, 60, 55, 50, 45]

# Sort methods based on performance
sorted_methods = [method for _, method in sorted(zip(performance_scores, method_names), reverse=True)]

# Choose a colormap (e.g., 'viridis')
cmap = plt.get_cmap('viridis')

# Normalize performance scores to the range [0, 1]
min_score = min(performance_scores)
max_score = max(performance_scores)
normalized_scores = [(score - min_score) / (max_score - min_score) for score in performance_scores]

# Create a figure and axes
fig, ax = plt.subplots()

# Define a consistent set of markers
markers = ['o', 's', 'D', 'v', '^', '<', '>', 'p', '*', 'H']

# Create 10 horizontal lines with different colors and consistent markers
for i, method in enumerate(sorted_methods):
    color = cmap(normalized_scores[i])
    x_values = np.arange(0, 10)  # Adjust the x-values as needed
    y_values = np.full(10, i)  # All lines are horizontal at the same y-level
    marker = markers[i % len(markers)]  # Cycle through consistent markers
    ax.plot(x_values, y_values, label=method, color=color, marker=marker, markersize=8)

# Add legend and labels
ax.legend()
ax.set_xlabel('X-axis')
ax.set_ylabel('Y-axis')
ax.set_title('Horizontal Lines with Consistent Markers Based on Method Performance')

# Show the figure
plt.show()
