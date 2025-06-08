# By DeepSeek

from PIL import Image

def generate_ascii_art(image_path, output_path, color_char_mapping, scale_factor=0.1, char_aspect_ratio=0.5):
    """
    生成字符画
    
    参数:
        image_path: 输入图片路径
        output_path: 输出文本文件路径
        color_char_mapping: 颜色到字符的映射字典，格式为 {(r,g,b): 'char'}
        scale_factor: 缩放因子，控制输出大小
        char_aspect_ratio: 字符的宽高比，用于校正输出比例
    """
    # 打开图片
    img = Image.open(image_path)
    
    # 计算新尺寸
    width, height = img.size
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor * char_aspect_ratio)
    
    # 调整图片大小
    img = img.resize((new_width, new_height))
    
    # 转换为RGB模式(如果还不是)
    img = img.convert('RGB')
    
    # 准备输出字符串
    ascii_art = []
    
    # 遍历每个像素
    for y in range(new_height):
        line = []
        for x in range(new_width):
            # 获取像素颜色
            r, g, b = img.getpixel((x, y))
            
            # 找到最接近的颜色映射
            closest_char = None
            min_distance = float('inf')
            
            for color, char in color_char_mapping.items():
                # 计算颜色距离(简单的欧几里得距离)
                distance = ((color[0] - r)**2 + 
                           (color[1] - g)**2 + 
                           (color[2] - b)**2)
                
                if distance < min_distance:
                    min_distance = distance
                    closest_char = char
            
            line.append(closest_char)
        ascii_art.append(''.join(line))
    
    # 写入输出文件
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(ascii_art))
    
    print(f"字符画已保存到 {output_path}")

# 示例使用
if __name__ == "__main__":
    # 定义颜色到字符的映射
    # 格式: {(R, G, B): '字符'}
    color_char_mapping = {
        (0, 0, 0): '@',        # 黑色
        (50, 50, 50): '#',     # 深灰
        (100, 100, 100): '*',  # 中灰
        (150, 150, 150): '+', # 浅灰
        (200, 200, 200): '-',  # 更浅灰
        (255, 255, 255): ' ',  # 白色
        (255, 0, 0): 'R',      # 红色
        (0, 255, 0): 'G',      # 绿色
        (0, 0, 255): 'B',      # 蓝色
        (255, 255, 0): 'Y',    # 黄色
        (255, 0, 255): 'M',    # 品红
        (0, 255, 255): 'C',    # 青色
    }
    
    # 你可以根据需要添加更多颜色映射
    
    # 生成字符画
    generate_ascii_art(
        image_path='incenterOS.png',       # 输入图片路径
        output_path='output.txt',     # 输出文本文件路径
        color_char_mapping=color_char_mapping,
        scale_factor=1,            # 调整这个值改变输出大小
        char_aspect_ratio=1        # 调整这个值校正宽高比
    )