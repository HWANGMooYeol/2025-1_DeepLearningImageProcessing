import cv2 as cv
import numpy as np

cap = cv.VideoCapture('LAB3_Video.mp4')

# set ROI
x, y, w, h = 0, 400, 460, 680

while cap.isOpened():
    ret, src = cap.read()
    if not ret:
        break

    roi = src[y:y+h, x:x+w]
    r_plane = roi[:, :, 2]
    r_blurred = cv.GaussianBlur(r_plane, (5, 5), 0)

    # Define additional blur regions
    blur_regions = [
        (376, 476, 39, 155),
        (97, 585, 37, 115),
        (363, 807, 29, 34),
        (331, 758, 19, 21),
        (390, 880, 50, 71),
        (170, 1010, 172, 38),
        (420, 781, 43, 38),
        (373, 858, 14, 17),
        (386, 813, 12, 22),
        (370, 860, 17, 16),
        (207, 959, 77, 35)
    ]

    for blur_x, blur_y, blur_w, blur_h in blur_regions:
        rel_x = blur_x - x
        rel_y = blur_y - y
        if 0 <= rel_x < w and 0 <= rel_y < h:
            sub_region = r_blurred[rel_y:rel_y+blur_h, rel_x:rel_x+blur_w]
            sub_blurred = cv.GaussianBlur(sub_region, (27, 27), 0)
            r_blurred[rel_y:rel_y+blur_h, rel_x:rel_x+blur_w] = sub_blurred
            #cv.rectangle(src, (blur_x, blur_y), (blur_x + blur_w, blur_y + blur_h), (0, 0, 255), 2)

    # Edge detection
    edges = cv.Canny(r_blurred, 90, 110)

    # Find edge coordinates 
    points = np.column_stack(np.where(edges > 0))  # (y, x)

    if len(points) > 0:
        ys, xs = points[:, 0], points[:, 1]
        sorted_idx = np.argsort(xs)
        xs = xs[sorted_idx]
        ys = ys[sorted_idx]

        try:
            coeffs = np.polyfit(xs, ys, 2)
            poly = np.poly1d(coeffs)
            curve_x = np.arange(0, w)
            curve_y = poly(curve_x).astype(np.int32)

            # Draw the curve
            for i in range(1, len(curve_x)):
                pt1 = (curve_x[i-1] + x, curve_y[i-1] + y)
                pt2 = (curve_x[i] + x, curve_y[i] + y)
                cv.line(src, pt1, pt2, (0, 255, 0), 2)

            # Calculate the vertex y-coordinate of the curve
            vertex_x = -coeffs[1] / (2 * coeffs[0])
            vertex_y = poly(vertex_x)
            vertex_y_global = int(vertex_y + y)
            distance_from_bottom = src.shape[0] - vertex_y_global

            # Determine the level
            if distance_from_bottom > 250:
                level = 1
            elif 120 <= distance_from_bottom <= 250:
                level = 2
            else:
                level = 3

            # Draw reference lines
            level2_y = src.shape[0] - 250
            level3_y = src.shape[0] - 120


            # Draw dashed reference lines
            for i in range(0, src.shape[1], 20):
                cv.line(src, (i, level2_y), (i+10, level2_y), (0, 255, 0), 1)          # Level 2 green dash
                cv.line(src, (i, level3_y), (i+10, level3_y), (255, 255, 0), 1)        # Level 3 blue dash

            # Display the text box
            score_text = f"Score : {distance_from_bottom:.2f}"
            level_text = f"Level : {level}"
            box_x, box_y = 900, 150
            line_height = 30
            cv.rectangle(src, (box_x, box_y), (box_x + 200, box_y + 2 * line_height + 20), (0, 255, 0), 2)
            font = cv.FONT_HERSHEY_SIMPLEX
            cv.putText(src, score_text, (box_x + 10, box_y + line_height), font, 0.7, (255, 255, 255), 1)
            cv.putText(src, level_text, (box_x + 10, box_y + 2 * line_height), font, 0.7, (255, 255, 255), 1)

        except np.RankWarning:
            pass

    output = np.zeros_like(src)
    output[y:y+h, x:x+w] = cv.cvtColor(edges, cv.COLOR_GRAY2BGR)

    # Display the output
    cv.imshow('Original Image with Curve', src)
    #cv.imshow('Red Blurred Image', r_blurred)
    #cv.imshow('Edge with Black Background', output)

    if cv.waitKey(25) & 0xFF == 27:
        break

cap.release()
cv.destroyAllWindows()